#pragma once

#include "../Core/Object.h"
#include "../Core/Mutex.h"
#include "../Container/List.h"

#include "IPCTypes.h"

namespace Atomic
{

class IPCBroker;
class IPCWorker;

struct QueuedEvent
{
    StringHash eventType_;
    VariantMap eventData_;
};

class IPC : public Object
{
    OBJECT(IPC);

public:

    /// Construct.
    IPC(Context* context);
    /// Destruct.
    virtual ~IPC();

    // queues an event from a worker or broker receiving thread
    void QueueEvent(StringHash eventType, VariantMap& eventData);

    // for a child worker process
    bool InitWorker(IPCHandle fd1, IPCHandle fd2);

    // spawn a worker process
    IPCBroker* SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory = "");

    // worker -> broker
    void SendEventToBroker(StringHash eventType);
    void SendEventToBroker(StringHash eventType, VariantMap& eventData);

private:

    // processes queued events
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    mutable Mutex eventMutex_;

    List<QueuedEvent> queuedEvents_;

    Vector<SharedPtr<IPCBroker> > brokers_;

    // valid on child
    SharedPtr<IPCWorker> worker_;

};

}
