
#include "../IO/Log.h"

#include "IPCChannel.h"

namespace Atomic
{

IPCChannel::IPCChannel(Context* context, unsigned id) : Object(context),
    id_(id)
{
    ipc_ = GetSubsystem<IPC>();
    currentHeader_.messageType_ = IPC_MESSAGE_UNDEFINED;
}

IPCChannel::~IPCChannel()
{

}

void IPCChannel::PostMessage(StringHash eventType, VariantMap &eventData)
{
    IPCMessageEvent msgEvent;
    msgEvent.DoSend(transport_, id_, eventType, eventData);
}

bool IPCChannel::Receive()
{
    size_t sz = 0;
    const char* data = transport_.Receive(&sz);

    if (!data)
    {
        // error
        return false;
    }

    if (!sz)
        return true;

    dataBuffer_.Seek(dataBuffer_.GetSize());
    dataBuffer_.Write(data, sz);
    dataBuffer_.Seek(0);

    while (true)
    {
        if (currentHeader_.messageType_ == IPC_MESSAGE_UNDEFINED &&
                dataBuffer_.GetSize() - dataBuffer_.GetPosition() < sizeof(IPCMessageHeader))
        {
            return true;
        }

        if (currentHeader_.messageType_ == IPC_MESSAGE_UNDEFINED)
        {
            dataBuffer_.Read(&currentHeader_, sizeof(IPCMessageHeader));
        }

        if (currentHeader_.messageSize_ <= dataBuffer_.GetSize() - dataBuffer_.GetPosition())
        {
            MemoryBuffer buffer(dataBuffer_.GetData() + dataBuffer_.GetPosition(), currentHeader_.messageSize_);
            dataBuffer_.Seek( dataBuffer_.GetPosition() + currentHeader_.messageSize_);
            currentHeader_.messageType_ = IPC_MESSAGE_UNDEFINED;

            IPCMessageEvent event;
            StringHash eventType;
            VariantMap eventData;
            unsigned id;
            event.DoRead(buffer, id, eventType, eventData);
            ipc_->QueueEvent(id, eventType, eventData);
        }

        if (dataBuffer_.IsEof())
        {
            dataBuffer_.Clear();
        }

        if (dataBuffer_.GetPosition() == 0)
            break;

        VectorBuffer newBuffer;
        newBuffer.Write(dataBuffer_.GetData() + dataBuffer_.GetPosition(), dataBuffer_.GetSize() - dataBuffer_.GetPosition());
        newBuffer.Seek(0);
        dataBuffer_ = newBuffer;
    }

    return true;

}


}
