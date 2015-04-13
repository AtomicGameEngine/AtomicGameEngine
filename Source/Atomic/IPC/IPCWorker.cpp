
#include "../IO/Log.h"

#include "IPCWorker.h"
#include "IPCMessage.h"
#include "IPCUnix.h"

namespace Atomic
{

IPCWorker::IPCWorker(int fd, Context* context) : IPCChannel(context),
    fd_(fd)
{
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

void IPCWorker::ThreadFunction()
{

    while(shouldRun_)
    {
        if (!Receive())
        {
            Stop();
            break;
        }
    }

}

}
