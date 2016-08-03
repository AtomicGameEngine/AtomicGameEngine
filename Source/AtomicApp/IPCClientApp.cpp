//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/IO/IOEvents.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCWorker.h>

#include "IPCClientApp.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Atomic
{

    IPCClientApp::IPCClientApp(Context* context) :
        AppBase(context),
        brokerActive_(false)
    {
        fd_[0] = INVALID_IPCHANDLE_VALUE;
        fd_[1] = INVALID_IPCHANDLE_VALUE;
    }

    IPCClientApp::~IPCClientApp()
    {
    }

    void IPCClientApp::Setup()
    {
        AppBase::Setup();

        ipc_ = new IPC(context_);
        context_->RegisterSubsystem(ipc_);

        SubscribeToEvent(E_LOGMESSAGE, ATOMIC_HANDLER(IPCClientApp, HandleLogMessage));
    }

    void IPCClientApp::HandleIPCInitialize(StringHash eventType, VariantMap& eventData)
    {
        brokerActive_ = true;
    }

    bool IPCClientApp::Initialize(const Vector<String>& arguments)
    {

        int id = -1;
        if (!IPC::ProcessArguments(arguments, id, fd_[0], fd_[1]))
            return false;

        SubscribeToEvent(E_IPCINITIALIZE, ATOMIC_HANDLER(IPCClientApp, HandleIPCInitialize));
        ipc_->InitWorker((unsigned)id, fd_[0], fd_[1]);

        return true;
    }


    void IPCClientApp::HandleLogMessage(StringHash eventType, VariantMap& eventData)
    {
        using namespace LogMessage;

        if (brokerActive_)
        {

            if (ipc_.Null())
                return;

            VariantMap logEvent;
            logEvent[IPCWorkerLog::P_LEVEL] = eventData[P_LEVEL].GetInt();
            logEvent[IPCWorkerLog::P_MESSAGE] = eventData[P_MESSAGE].GetString();
            ipc_->SendEventToBroker(E_IPCWORKERLOG, logEvent);
        }

    }

}
