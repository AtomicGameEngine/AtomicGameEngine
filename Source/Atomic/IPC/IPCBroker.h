
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

    bool Update();

private:

    bool SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory = "");

    // broker instantiates the pipe pair
    PipePair pp_;

};

}
