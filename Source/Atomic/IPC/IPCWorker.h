
#pragma once

#include "IPC.h"
#include "IPCChannel.h"

namespace Atomic
{

class IPCWorker : public IPCChannel
{
    OBJECT(IPCWorker);

public:
    /// Construct.
    IPCWorker(int fd, Context* context);
    /// Destruct.
    virtual ~IPCWorker();

    void ThreadFunction();

private:

    int fd_;

};

}
