
#pragma once

#include "../Core/Variant.h"
#include "../IO/VectorBuffer.h"
#include "../IO/MemoryBuffer.h"

#include "IPCUnix.h"

namespace Atomic
{

const unsigned IPC_MESSAGE_EVENT = 0;

class IPCMessageEvent
{

public:

    bool DoRead(VectorBuffer& buffer, StringHash& eventType, VariantMap& eventData)
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

        unsigned sz = buffer.GetSize();
        transport.Write(&IPC_MESSAGE_EVENT, sizeof(unsigned));
        transport.Write(&sz, sizeof(unsigned));
        transport.Write(buffer.GetData(), sz);

        return true;
    }
};

}
