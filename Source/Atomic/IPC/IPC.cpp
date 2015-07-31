
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

namespace Atomic
{

IPC::IPC(Context* context) : Object(context),
    workerChannelID_(0)
{
    SubscribeToEvent(E_UPDATE, HANDLER(IPC, HandleUpdate));
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

void IPC::HandleUpdate(StringHash eventType, VariantMap& eventData)
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
