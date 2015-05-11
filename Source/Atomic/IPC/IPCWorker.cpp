
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

IPCWorker::IPCWorker(IPCHandle fd, Context* context) : IPCChannel(context),
    fd_(fd)
{

#ifdef ATOMIC_PLATFORM_WINDOWS
    otherProcess_ = new IPCProcess(context_, INVALID_IPCHANDLE_VALUE, fd_, INVALID_IPCHANDLE_VALUE);
#else
    otherProcess_ = new IPCProcess(context_, -1, fd, getppid());
#endif

    if (!transport_.OpenClient(fd_))
    {
        LOGERRORF("Unable to open IPC transport fd = %i", fd_);
        shouldRun_ = false;
        return;
    }

    LOGERRORF("Opened IPC transport fd = %i", fd_);

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
