
#pragma once

#include <SimpleIPC/ipc_pipe.h>

#include "../Core/Mutex.h"
#include "../Core/Thread.h"
#include "../Core/Object.h"

namespace Atomic
{

class IPCBroker : public Object, public Thread
{
    OBJECT(IPCBroker);

public:
    /// Construct.
    IPCBroker(Context* context);
    /// Destruct.
    virtual ~IPCBroker();

};

}
