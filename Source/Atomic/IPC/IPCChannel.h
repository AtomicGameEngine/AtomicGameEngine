

#pragma once

#include "../Core/Mutex.h"
#include "../Core/Thread.h"
#include "../Core/Object.h"

#include "IPC.h"
#include "IPCMessage.h"
#include "IPCUnix.h"

namespace Atomic
{

class IPCChannel : public Object, public Thread
{
    OBJECT(IPCChannel)

public:

    IPCChannel(Context* context);
    virtual ~IPCChannel();

    virtual void ThreadFunction() {}

    bool Receive();

    void PostMessage(StringHash eventType, VariantMap& eventData);

protected:

    // for access from thread
    WeakPtr<IPC> ipc_;

    // for brokers this is the worker process
    // for workers, the broker process
    SharedPtr<IPCProcess> otherProcess_;

    PipeTransport transport_;
    IPCMessageHeader currentHeader_;
    VectorBuffer dataBuffer_;

};

}
