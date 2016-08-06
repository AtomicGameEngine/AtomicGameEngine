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

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Container/List.h>

namespace Atomic
{
    ATOMIC_EVENT(E_IPCCMD, IPCCmd)
    {
        ATOMIC_PARAM(P_COMMAND, Command);
        ATOMIC_PARAM(P_ID, ID); // unsigned
    }

    ATOMIC_EVENT(E_IPCCMDRESULT, IPCCmdResult)
    {
        ATOMIC_PARAM(P_COMMAND, Command);
        ATOMIC_PARAM(P_ID, ID); // unsigned
    }

    class IPCBroker;

    /// IPCResultHandler
    class IPCResultHandler : public Object
    {
        ATOMIC_OBJECT(IPCResultHandler, Object)

    public:
        /// Construct.
        IPCResultHandler(Context* context);
        /// Destruct.
        virtual ~IPCResultHandler();

        virtual void HandleResult(unsigned cmdID, const VariantMap& cmdResult) = 0;

    private:

    };

    /// IPCServer
    class IPCServer : public Object
    {
        ATOMIC_OBJECT(IPCServer, Object)

    public:
        /// Construct.
        IPCServer(Context* context);
        /// Destruct.
        virtual ~IPCServer();

        virtual bool Start() = 0;

        unsigned QueueCommand(IPCResultHandler* handler, const VariantMap& cmdMap);
        bool GetBrokerEnabled() const;

    protected:

        bool StartInternal(const String& exec, const Vector<String>& args);

    private:

        struct IPCCommand
        {
            unsigned id_;
            WeakPtr<IPCResultHandler> handler_;
            VariantMap cmdMap_;
        };

        void HandleUpdate(StringHash eventType, VariantMap & eventData);

        void HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData);
        void HandleIPCWorkerLog(StringHash eventType, VariantMap& eventData);
        void HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData);

        void HandleIPCCmdResult(StringHash eventType, VariantMap& eventData);

        String clientExecutable_;

        SharedPtr<IPCBroker> serverBroker_;
        bool brokerEnabled_;

        List<IPCCommand> cmdQueue_;
        List<IPCCommand> cmdProcess_;

        static unsigned cmdID_;

    };

}
