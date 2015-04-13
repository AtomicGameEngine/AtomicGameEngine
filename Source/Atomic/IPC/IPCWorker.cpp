
#include "../IO/Log.h"

#include "IPCWorker.h"
#include "IPCMessage.h"
#include "IPCUnix.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#else
    #include <unistd.h>
#endif

namespace Atomic
{

IPCWorker::IPCWorker(int fd, Context* context) : IPCChannel(context),
    fd_(fd)
{

    otherProcess_ = new IPCProcess(context_, -1, fd, getppid());

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
