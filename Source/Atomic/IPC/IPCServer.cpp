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

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include <Atomic/Core/CoreEvents.h>

#include "IPCServer.h"

namespace Atomic
{


    unsigned IPCServer::cmdID_ = 1;

    IPCResultHandler::IPCResultHandler(Context* context) :
        Object(context)
    {

    }

    IPCResultHandler::~IPCResultHandler()
    {

    }

    IPCServer::IPCServer(Context* context) :
        Object(context),
        brokerEnabled_(false)
    {

    }

    IPCServer::~IPCServer()
    {

    }

    void IPCServer::HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData)
    {
        VariantMap startupData;
        serverBroker_->PostMessage(E_IPCINITIALIZE, startupData);
        brokerEnabled_ = true;

        SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(IPCServer, HandleUpdate));

        SubscribeToEvent(serverBroker_, E_IPCCMDRESULT, ATOMIC_HANDLER(IPCServer, HandleIPCCmdResult));
    }

    void IPCServer::HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData)
    {
        if (eventData[IPCWorkerExit::P_BROKER] == serverBroker_)
        {
            serverBroker_ = 0;
            brokerEnabled_ = false;
        }
    }

    void IPCServer::HandleIPCWorkerLog(StringHash eventType, VariantMap& eventData)
    {
        using namespace IPCWorkerLog;

        // convert to a server log

        VariantMap serverLogData;

        serverLogData["message"] = eventData[P_MESSAGE].GetString();
        serverLogData["level"] = eventData[P_LEVEL].GetInt();

        SendEvent("IPCServerLog", serverLogData);

    }

    bool IPCServer::StartInternal(const String& exec, const Vector<String>& args)
    {

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        clientExecutable_ = exec;

        if (!clientExecutable_.Length() || !fileSystem->FileExists(clientExecutable_))
        {
            ATOMIC_LOGERRORF("IPCServer::Start - Client Executable does not exist: %s", clientExecutable_.CString());
            return false;
        }

        String dump;
        dump.Join(args, " ");

        ATOMIC_LOGDEBUGF("Launching Broker %s %s", clientExecutable_.CString(), dump.CString());

        IPC* ipc = GetSubsystem<IPC>();
        serverBroker_ = ipc->SpawnWorker(clientExecutable_, args);

        if (serverBroker_)
        {
            SubscribeToEvent(serverBroker_, E_IPCWORKERSTART, ATOMIC_HANDLER(IPCServer, HandleIPCWorkerStarted));
            SubscribeToEvent(serverBroker_, E_IPCWORKEREXIT, ATOMIC_HANDLER(IPCServer, HandleIPCWorkerExit));
            SubscribeToEvent(serverBroker_, E_IPCWORKERLOG, ATOMIC_HANDLER(IPCServer, HandleIPCWorkerLog));
        }
        else
        {
            ATOMIC_LOGERRORF("Error Spawning Broker %s %s", clientExecutable_.CString(), dump.CString());
        }

        return serverBroker_.NotNull();

    }

    bool IPCServer::GetBrokerEnabled() const
    {
        return brokerEnabled_;
    }

    void IPCServer::HandleIPCCmdResult(StringHash eventType, VariantMap& eventData)
    {
        using namespace IPCCmdResult;

        unsigned id = eventData[P_ID].GetUInt();

        List<IPCCommand>::Iterator itr = cmdProcess_.Begin();

        bool found = false;
        while (itr != cmdProcess_.End())
        {
            if ((*itr).id_ == id)
            {
                IPCCommand cmd = *itr;

                cmdProcess_.Erase(itr);

                found = true;

                if (cmd.handler_.Expired())
                {
                    ATOMIC_LOGERROR("IPCServer::HandleIPCNETCmdResult - IPCNETResult for expired client");
                    break;
                }

                cmd.handler_->HandleResult(cmd.id_, eventData);

                break;
            }

            itr++;

        }

        if (!found)
        {
            ATOMIC_LOGERRORF("IPCServer::HandleIPCNETCmdResult - IPCNETResult command %u not found in process queue", id);
        }

    }


    unsigned IPCServer::QueueCommand(IPCResultHandler* handler, const VariantMap& cmdMap)
    {
        IPCCommand cmd;

        cmd.id_ = cmdID_++;
        cmd.handler_ = handler;
        cmd.cmdMap_ = cmdMap;
        cmd.cmdMap_[IPCCmd::P_ID] = cmd.id_;

        cmdQueue_.Push(cmd);

        return cmd.id_;
    }

    void IPCServer::HandleUpdate(StringHash eventType, VariantMap& eventData)
    {

        if (!cmdQueue_.Size())
            return;

        IPCCommand cmd = cmdQueue_.Front();
        cmdQueue_.PopFront();

        String cmdString = cmd.cmdMap_["command"].GetString();

        if (!serverBroker_)
        {
            ATOMIC_LOGERRORF("IPCServer::HandleUpdate - null player broker for command: %s", cmdString.CString());
            return;
        }

        cmdProcess_.Push(cmd);

        serverBroker_->PostMessage(E_IPCCMD, cmd.cmdMap_);

    }


}
