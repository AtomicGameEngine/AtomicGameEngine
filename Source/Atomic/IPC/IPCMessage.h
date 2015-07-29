
#pragma once

#include "../Core/Variant.h"
#include "../IO/VectorBuffer.h"
#include "../IO/MemoryBuffer.h"

#ifndef ATOMIC_PLATFORM_WINDOWS
#include "IPCUnix.h"
#else
#include "IPCWindows.h"
#endif

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

    bool DoRead(MemoryBuffer& buffer, unsigned& id, StringHash& eventType, VariantMap& eventData)
    {
        id = buffer.ReadUInt();
        eventType = buffer.ReadStringHash();
        eventData.Clear();
        eventData = buffer.ReadVariantMap();
        return true;
    }

    bool DoSend(PipeTransport& transport, unsigned id, const StringHash& eventType, const VariantMap& eventData)
    {
        VectorBuffer buffer;
        buffer.WriteUInt(id);
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
