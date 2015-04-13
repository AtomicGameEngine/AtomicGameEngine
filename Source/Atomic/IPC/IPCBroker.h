
#pragma once

#include "IPCChannel.h"

namespace Atomic
{

class IPCProcess;

class IPCBroker : public IPCChannel
{
    OBJECT(IPCBroker);

public:
    /// Construct.
    IPCBroker(Context* context);
    /// Destruct.
    virtual ~IPCBroker();

    void ThreadFunction();

    bool SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory = "");

private:

    SharedPtr<IPCProcess> workerProcess_;


};

}
