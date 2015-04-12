
#pragma once

#include "../Core/Mutex.h"
#include "../Core/Thread.h"
#include "../Core/Object.h"

namespace Atomic
{

class IPCWorker : public Object, public Thread
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
