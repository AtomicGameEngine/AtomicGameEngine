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

#include "../Core/Timer.h"
#include "../IO/Log.h"

#include "IPCWorker.h"
#include "IPCMessage.h"


#ifdef ATOMIC_PLATFORM_WINDOWS
#include "IPCWindows.h"
#else
#include "IPCUnix.h"
#include <unistd.h>
#endif

namespace Atomic
{

IPCWorker::IPCWorker(Context* context, IPCHandle clientRead, IPCHandle clientWrite, unsigned id) : IPCChannel(context, id)
{
#ifndef ATOMIC_PLATFORM_WINDOWS
	assert(0); // wrong constructor
#else
	otherProcess_ = new IPCProcess(context_, clientRead, clientWrite, INVALID_IPCHANDLE_VALUE);

	if (!transport_.OpenClient(clientRead, clientWrite))
	{
        ATOMIC_LOGERRORF("Unable to open IPC transport clientRead = %i", clientRead);
		shouldRun_ = false;
		return;
	}

    ATOMIC_LOGERRORF("Opened IPC transport fd = %i", clientRead);

#endif

}

IPCWorker::IPCWorker(Context* context, IPCHandle fd, unsigned id) : IPCChannel(context, id),
    clientRead_(fd),
	clientWrite_(fd)
{

#ifdef ATOMIC_PLATFORM_WINDOWS
	assert(0); // wrong constructor
#else
    otherProcess_ = new IPCProcess(context_, -1, clientRead_, getppid());

    if (!transport_.OpenClient(clientRead_))
    {
        ATOMIC_LOGERRORF("Unable to open IPC transport fd = %i", clientRead_);
        shouldRun_ = false;
        return;
    }

    ATOMIC_LOGERRORF("Opened IPC transport fd = %i", clientRead_);
#endif
}

IPCWorker::~IPCWorker()
{

}

bool IPCWorker::Update()
{
    if (otherProcess_.Null())
        return false;

    if (!shouldRun_)
    {
        Stop();
        return false;
    }

    return true;
}

void IPCWorker::ThreadFunction()
{
    while (shouldRun_)
    {

// On windows we use a job object to control process lifetime, we don't have a 
// parent pid (these change and are reused on Windows, so we would need to DuplicateHandle and pass
// to child on command line
#ifndef ATOMIC_PLATFORM_WINDOWS
        if (!otherProcess_->IsRunning())
        {
            break;
        }
#endif

        if (!Receive())
        {
           break;
        }

        Time::Sleep(10);
    }

    shouldRun_ = false;
}

}
