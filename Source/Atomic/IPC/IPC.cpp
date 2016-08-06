//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#if defined(ATOMIC_PLATFORM_OSX) || defined(ATOMIC_PLATFORM_LINUX)
#include <unistd.h>
#endif

#include "../Core/CoreEvents.h"
#include "../Engine/Engine.h"
#include "../IO/Log.h"

#include "IPCBroker.h"
#include "IPCWorker.h"
#include "IPC.h"
#include "IPCEvents.h"

#if defined(ATOMIC_PLATFORM_WINDOWS)

#include <windows.h>
#undef PostMessage

#endif

namespace Atomic
{

IPC::IPC(Context* context) : Object(context),
    workerChannelID_(0)
{
    SubscribeToEvent(E_BEGINFRAME, ATOMIC_HANDLER(IPC, HandleBeginFrame));

#ifdef ATOMIC_PLATFORM_WINDOWS

    jobHandle_ = CreateJobObject(NULL, NULL);
    if (!jobHandle_)
    {
        ATOMIC_LOGERROR("IPC::IPC - Unable to create IPC job");
    }
    else
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };

        // Configure all child processes associated with the job to terminate when main process is closed
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        if (0 == SetInformationJobObject(jobHandle_, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)))
        {
            ATOMIC_LOGERROR("IPC::IPC - Unable set job information");
            jobHandle_ = 0;
        }
    }

#endif
}

IPC::~IPC()
{
    for (unsigned i = 0; i < brokers_.Size(); i++)
        brokers_[i]->Stop();

    brokers_.Clear();

    if (worker_.NotNull())
        worker_->Stop();

    worker_ = 0;
}

bool IPC::InitWorker(unsigned id, IPCHandle fd1, IPCHandle fd2)
{
    workerChannelID_ = id;

#ifndef ATOMIC_PLATFORM_WINDOWS
    // close server fd
    close(fd1);
    worker_ = new IPCWorker(context_, fd2, id);
#else
    worker_ = new IPCWorker(context_, fd1, fd2, id);
#endif

    worker_->Run();

    SendEventToBroker(E_IPCWORKERSTART);

    return true;
}

bool IPC::ProcessArguments(const Vector<String>& arguments, int& id, IPCHandle& fd0, IPCHandle& fd1)
{
    id = -1;
    fd0 = INVALID_IPCHANDLE_VALUE;
    fd1 = INVALID_IPCHANDLE_VALUE;

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1)
        {
            String argument = arguments[i].ToLower();

            if (argument.StartsWith("--ipc-id="))
            {
                Vector<String> idc = argument.Split(argument.CString(), '=');
                if (idc.Size() == 2)

                    id = ToInt(idc[1].CString());
            }

            else if (argument.StartsWith("--ipc-server=") || argument.StartsWith("--ipc-client="))
            {
                ATOMIC_LOGINFOF("Starting IPCWorker %s", argument.CString());

                Vector<String> ipc = argument.Split(argument.CString(), '=');

                if (ipc.Size() == 2)
                {
                    if (argument.StartsWith("--ipc-server="))
                    {
#ifdef ATOMIC_PLATFORM_WINDOWS
                        // clientRead
                        WString wipc(ipc[1]);
                        HANDLE pipe = reinterpret_cast<HANDLE>(_wtoi64(wipc.CString()));
                        fd0 = pipe;
#else
                        int fd = ToInt(ipc[1].CString());
                        fd0 = fd;
#endif
                    }
                    else
                    {
#ifdef ATOMIC_PLATFORM_WINDOWS
                        // clientWrite
                        WString wipc(ipc[1]);
                        HANDLE pipe = reinterpret_cast<HANDLE>(_wtoi64(wipc.CString()));
                        fd1 = pipe;
#else
                        int fd = ToInt(ipc[1].CString());
                        fd1 = fd;
#endif
                    }

                }

            }

        }
    }

    if (id > 0 && fd0 != INVALID_IPCHANDLE_VALUE && fd1 != INVALID_IPCHANDLE_VALUE)
    {
        return true;
    }

    return false;

}

IPCBroker* IPC::SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    SharedPtr<IPCBroker> broker(new IPCBroker(context_));

    if (broker->SpawnWorker(command, args, initialDirectory))
    {
        brokers_.Push(broker);
        return broker;
    }

    return 0;
}

void IPC::SendEventToBroker(StringHash eventType)
{
    SendEventToBroker(eventType, GetEventDataMap());
}

void IPC::SendEventToBroker(StringHash eventType, VariantMap& eventData)
{
    if (worker_.NotNull())
    {
        worker_->PostMessage(eventType, eventData);
    }
}

void IPC::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    // If we're a worker, if update fails, time to exit
    if (worker_.NotNull())
    {
        if (!worker_->Update())
        {
            worker_ = 0;
            GetSubsystem<Engine>()->Exit();
            return;
        }
    }

    // Update brokers
    Vector<IPCBroker*> remove;

    for (unsigned i = 0; i < brokers_.Size(); i++)
    {
        SharedPtr<IPCBroker>& broker = brokers_[i];
        if (!broker->Update())
        {
            VariantMap brokerData;
            brokerData[IPCWorkerExit::P_BROKER] = broker;
            brokerData[IPCWorkerExit::P_EXITCODE] = 0;
            broker->SendEvent(E_IPCWORKEREXIT, brokerData);
            remove.Push(broker);
        }
    }

    for (unsigned i = 0; i < remove.Size(); i++)
    {
        brokers_.Remove(SharedPtr<IPCBroker>(remove[i]));
    }

    eventMutex_.Acquire();

    for (List<QueuedEvent>::Iterator itr = queuedEvents_.Begin(); itr != queuedEvents_.End(); itr++)
    {
        unsigned channelID = (*itr).channelID_;
        StringHash qeventType =  (*itr).eventType_;
        VariantMap& qeventData =  (*itr).eventData_;

        bool sent = false;

        for (unsigned i = 0; i < brokers_.Size(); i++)
        {
            SharedPtr<IPCBroker>& broker = brokers_[i];
            if (broker->GetID() == channelID) {

                broker->SendEvent(qeventType, qeventData);
                sent = true;
                break;
            }
        }

        if (!sent)
            SendEvent(qeventType, qeventData);


    }

    queuedEvents_.Clear();

    eventMutex_.Release();
}

void IPC::QueueEvent(unsigned id, StringHash eventType, VariantMap& eventData)
{
    eventMutex_.Acquire();

    QueuedEvent event;
    event.channelID_ = id;
    event.eventType_ = eventType;
    event.eventData_ = eventData;
    queuedEvents_.Push(event);

    eventMutex_.Release();
}


}
