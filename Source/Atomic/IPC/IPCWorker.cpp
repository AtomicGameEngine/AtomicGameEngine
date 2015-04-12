
#include "IPCWorker.h"

#include "../IO/Log.h"

#include <SimpleIPC/ipc_channel.h>
#include <SimpleIPC/ipc_codec.h>
#include <SimpleIPC/ipc_msg_dispatch.h>

namespace Atomic
{

IPCWorker::IPCWorker(int fd, Context* context) : Object(context),
    fd_(fd)
{

}

IPCWorker::~IPCWorker()
{

}

void IPCWorker::ThreadFunction()
{

    PipeTransport transport;

    if (!transport.OpenClient(fd_))
    {
        LOGERRORF("Unable to open IPC transport fd = %i", fd_);
        shouldRun_ = false;
        return;
    }

    int answer = 42;
    size_t sz = sizeof(int);
    transport.Write(&answer, sz);

    LOGERRORF("Wrote to IPC transport fd = %i", fd_);

}

}
