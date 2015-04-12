
#pragma once

#include <Poco/Process.h>
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

    void ThreadFunction();

    bool SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory = "");
    bool SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory, const Poco::Process::Env& env);

private:

    PipeTransport transport_;
    PipePair pp_;

    Poco::ProcessHandle* workerHandle_;

};

}
