//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

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
