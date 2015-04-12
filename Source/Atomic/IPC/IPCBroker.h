
#pragma once

#include "../Core/Mutex.h"
#include "../Core/Thread.h"
#include "../Core/Object.h"

#include "IPCUnix.h"

namespace Atomic
{

class IPCProcess;

class IPCBroker : public Object, public Thread
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
    PipeTransport transport_;

};

}
