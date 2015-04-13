
#ifdef __APPLE__
#include <unistd.h>
#endif

#include "../Core/CoreEvents.h"
#include "../IO/Log.h"

#include "IPCBroker.h"
#include "IPCWorker.h"
#include "IPC.h"
#include "IPCEvents.h"

namespace Atomic
{

IPC::IPC(Context* context) : Object(context)
{
    SubscribeToEvent(E_UPDATE, HANDLER(IPC, HandleUpdate));
}

IPC::~IPC()
{

}

bool IPC::InitWorker(int fd1, int fd2)
{
    // close server fd
    close(fd1);

    worker_ = new IPCWorker(fd2, context_);
    worker_->Run();

    SendEventToBroker(E_IPCWORKERSTART);

    return true;
}

IPCBroker* IPC::SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    SharedPtr<IPCBroker> broker(new IPCBroker(context_));

    if (broker->SpawnWorker(command, args, initialDirectory))
    {
        brokers_.Push(broker);
        return broker;
    }

    return 0;
}

void IPC::SendEventToBroker(StringHash eventType)
{
    SendEventToBroker(eventType, GetEventDataMap());
}

void IPC::SendEventToBroker(StringHash eventType, VariantMap& eventData)
{
    if (worker_.NotNull())
    {
        worker_->PostMessage(eventType, eventData);
    }
}

void IPC::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    eventMutex_.Acquire();

    for (List<QueuedEvent>::Iterator itr = queuedEvents_.Begin(); itr != queuedEvents_.End(); itr++)
    {
        StringHash qeventType =  (*itr).eventType_;
        VariantMap& qeventData =  (*itr).eventData_;
        SendEvent(qeventType, qeventData);
    }

    queuedEvents_.Clear();

    eventMutex_.Release();
}

void IPC::QueueEvent(StringHash eventType, VariantMap& eventData)
{
    eventMutex_.Acquire();

    QueuedEvent event;
    event.eventType_ = eventType;
    event.eventData_ = eventData;
    queuedEvents_.Push(event);

    eventMutex_.Release();
}


}
