
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
    /// POSIX Constructor   
	IPCWorker(Context* context, IPCHandle fd, unsigned id);

	// Windows Constructor, two named pipes are used
	IPCWorker(Context* context, IPCHandle clientRead, IPCHandle clientWrite, unsigned id);
    
	/// Destruct.
    virtual ~IPCWorker();

    void ThreadFunction();

    bool Update();

private:

	// on unix will be the same
    IPCHandle clientRead_;
	IPCHandle clientWrite_;

};

}
