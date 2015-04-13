#pragma once

#include "../Core/Object.h"
#include "../Core/Mutex.h"

#include "../Container/List.h"

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
    IPC(Context* context, int fd1 = -1, int fd2 = -1);
    /// Destruct.
    virtual ~IPC();

    void QueueEvent(StringHash eventType, VariantMap& eventData);

    void SendEventToBroker(StringHash eventType);
    void SendEventToBroker(StringHash eventType, VariantMap& eventData);

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    mutable Mutex eventMutex_;

    List<QueuedEvent> queuedEvents_;

    Vector<SharedPtr<IPCBroker> > brokers_;

    // valid on child
    SharedPtr<IPCWorker> worker_;

};

}
