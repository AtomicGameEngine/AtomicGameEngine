
#pragma once

#include "IPCTypes.h"
#include "IPC.h"
#include "IPCChannel.h"

namespace Atomic
{

class IPCWorker : public IPCChannel
{
    OBJECT(IPCWorker);

public:
    /// Construct.
    IPCWorker(IPCHandle fd, Context* context);
    /// Destruct.
    virtual ~IPCWorker();

    void ThreadFunction();

    bool Update();

private:

    IPCHandle fd_;

};

}
