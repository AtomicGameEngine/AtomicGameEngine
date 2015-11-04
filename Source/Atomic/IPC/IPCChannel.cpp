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
    dataBuffer_.Write(data, (unsigned) sz);
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
