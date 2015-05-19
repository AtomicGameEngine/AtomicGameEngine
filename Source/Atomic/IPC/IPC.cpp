
#ifndef ATOMIC_PLATFORM_WINDOWS
#include <unistd.h>
#endif

#include "../Core/CoreEvents.h"
#include "../Engine/Engine.h"
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
    for (unsigned i = 0; i < brokers_.Size(); i++)
        brokers_[i]->Stop();

    brokers_.Clear();

    if (worker_.NotNull())
        worker_->Stop();

    worker_ = 0;
}

bool IPC::InitWorker(IPCHandle fd1, IPCHandle fd2)
{
    
#ifndef ATOMIC_PLATFORM_WINDOWS
    // close server fd
    close(fd1);
#endif

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
    // If we're a worker, if update fails, time to exit
    if (worker_.NotNull())
    {
        if (!worker_->Update())
        {
            worker_ = 0;
            GetSubsystem<Engine>()->Exit();
            return;
        }
    }

    // Update brokers
    Vector<IPCBroker*> remove;

    for (unsigned i = 0; i < brokers_.Size(); i++)
    {
        SharedPtr<IPCBroker>& broker = brokers_[i];
        if (!broker->Update())
        {
            VariantMap brokerData;
            brokerData[WorkerExit::P_BROKER] = broker;
            brokerData[WorkerExit::P_EXITCODE] = 0;
            broker->SendEvent(E_IPCWORKEREXIT, brokerData);
            remove.Push(broker);
        }
    }

    for (unsigned i = 0; i < remove.Size(); i++)
    {
        brokers_.Remove(SharedPtr<IPCBroker>(remove[i]));
    }

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
