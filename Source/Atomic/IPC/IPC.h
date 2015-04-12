#pragma once

#include "../Core/Object.h"

namespace Atomic
{

class IPCBroker;
class IPCWorker;

class IPC : public Object
{
    OBJECT(IPC);

public:

    /// Construct.
    IPC(Context* context);
    /// Destruct.
    virtual ~IPC();

private:

    Vector<SharedPtr<IPCBroker> > brokers_;
    Vector<SharedPtr<IPCWorker> > workers_;

};

}
