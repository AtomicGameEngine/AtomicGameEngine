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

#include "../Core/Mutex.h"
#include "../Core/Thread.h"
#include "../Core/Object.h"

#include "IPC.h"
#include "IPCMessage.h"
#include "IPCUnix.h"

#ifdef ATOMIC_PLATFORM_WINDOWS

// Windows defines PostMessage as PostMessgeA/W
#undef PostMessage

#endif

namespace Atomic
{

class IPCChannel : public Object, public Thread
{
    ATOMIC_OBJECT(IPCChannel, Object)

public:

    IPCChannel(Context* context, unsigned id);
    virtual ~IPCChannel();

    virtual void ThreadFunction() {}

    unsigned GetID() { return id_; }

    bool Receive();

    void PostMessage(StringHash eventType, VariantMap& eventData);

    IPCProcess* GetOtherProcess() { return otherProcess_; }

protected:

    unsigned id_;

    // for access from thread
    WeakPtr<IPC> ipc_;

    // for brokers this is the worker process
    // for workers, the broker process
    SharedPtr<IPCProcess> otherProcess_;

    PipeTransport transport_;
    IPCMessageHeader currentHeader_;
    VectorBuffer dataBuffer_;

};

}
