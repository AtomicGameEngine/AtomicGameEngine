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

#include "../Core/Object.h"
#include "../Core/Mutex.h"
#include "../Container/List.h"

#include "IPCTypes.h"

namespace Atomic
{

class IPCBroker;
class IPCWorker;

struct QueuedEvent
{
    unsigned channelID_;
    StringHash eventType_;
    VariantMap eventData_;
};

class IPC : public Object
{
    ATOMIC_OBJECT(IPC, Object);

public:

    /// Construct.
    IPC(Context* context);
    /// Destruct.
    virtual ~IPC();

    // queues an event from a worker or broker receiving thread
    void QueueEvent(unsigned id, StringHash eventType, VariantMap& eventData);

    // for a child worker process
    bool InitWorker(unsigned id, IPCHandle fd1, IPCHandle fd2);

    // spawn a worker process
    IPCBroker* SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory = "");

    // worker -> broker
    void SendEventToBroker(StringHash eventType);
    void SendEventToBroker(StringHash eventType, VariantMap& eventData);

    // Processes arg strings looking for ipc server/client handles, returns true if an IPC subprocess
    static bool ProcessArguments(const Vector<String>& arguments, int& id, IPCHandle& fd1, IPCHandle& fd2);

#ifdef ATOMIC_PLATFORM_WINDOWS
    IPCHandle GetJobHandle() const { return jobHandle_; }
#endif

private:

    // if non-zero we're a worked and this is out broker's channel id
    unsigned workerChannelID_;

    // processes queued events
    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);

    mutable Mutex eventMutex_;

    List<QueuedEvent> queuedEvents_;

    Vector<SharedPtr<IPCBroker> > brokers_;

    // valid on child
    SharedPtr<IPCWorker> worker_;

#ifdef ATOMIC_PLATFORM_WINDOWS
    IPCHandle jobHandle_;
#endif

};

}
