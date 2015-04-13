
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

IPC::IPC(Context* context, int fd1, int fd2) : Object(context)
{
    SubscribeToEvent(E_UPDATE, HANDLER(IPC, HandleUpdate));

    if (fd1 != -1 && fd2 != -1)
    {
        // close server fd
        close(fd1);

        worker_ = new IPCWorker(fd2, context_);
        worker_->Run();

        SendEventToBroker(E_IPCWORKERSTART);
    }

}

IPC::~IPC()
{

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
