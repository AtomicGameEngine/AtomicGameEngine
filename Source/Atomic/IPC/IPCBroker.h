
#pragma once

#include "IPCChannel.h"

namespace Atomic
{

class IPCProcess;

class IPCBroker : public IPCChannel
{
    friend class IPC;

    OBJECT(IPCBroker);

public:
    /// Construct.
    IPCBroker(Context* context);
    /// Destruct.
    virtual ~IPCBroker();

    void ThreadFunction();

private:

    bool SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory = "");

    SharedPtr<IPCProcess> workerProcess_;

};

}
