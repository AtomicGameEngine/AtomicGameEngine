
#pragma once

#include "../Core/Variant.h"
#include "../IO/VectorBuffer.h"
#include "../IO/MemoryBuffer.h"

#include "IPCUnix.h"

namespace Atomic
{

const unsigned IPC_MESSAGE_UNDEFINED = 0;
const unsigned IPC_MESSAGE_EVENT = 1;

struct IPCMessageHeader
{
    unsigned messageType_;
    unsigned messageSize_;
};

class IPCMessageEvent
{

public:

    bool DoRead(MemoryBuffer& buffer, StringHash& eventType, VariantMap& eventData)
    {
        eventType = buffer.ReadStringHash();
        eventData.Clear();
        eventData = buffer.ReadVariantMap();
        return true;
    }

    bool DoSend(PipeTransport& transport, const StringHash& eventType, const VariantMap& eventData)
    {
        VectorBuffer buffer;
        buffer.WriteStringHash(eventType);
        buffer.WriteVariantMap(eventData);

        IPCMessageHeader header;
        header.messageType_ = IPC_MESSAGE_EVENT;
        header.messageSize_ = buffer.GetSize();

        transport.Write(&header, sizeof(IPCMessageHeader));
        transport.Write(buffer.GetData(), header.messageSize_);

        return true;
    }
};

}
