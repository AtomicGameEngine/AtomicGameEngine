
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
		LOGERRORF("Unable to open IPC transport clientRead = %i", clientRead);
		shouldRun_ = false;
		return;
	}

	LOGERRORF("Opened IPC transport fd = %i", clientRead);

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
        LOGERRORF("Unable to open IPC transport fd = %i", clientRead_);
        shouldRun_ = false;
        return;
    }

    LOGERRORF("Opened IPC transport fd = %i", clientRead_);
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
        if (!otherProcess_->IsRunning())
        {
            break;
        }

        if (!Receive())
        {
           break;
        }
    }

    shouldRun_ = false;
}

}
