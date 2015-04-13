
#include <unistd.h>

#include "../Core/StringUtils.h"
#include "../IO/Log.h"

#include "IPCUnix.h"
#include "IPCBroker.h"


namespace Atomic
{

IPCBroker::IPCBroker(Context* context) : IPCChannel(context)
{    

}

IPCBroker::~IPCBroker()
{
}

void IPCBroker::ThreadFunction()
{
    while (shouldRun_)
    {
        if (!Receive())
        {
            Stop();
            break;
        }
    }

    shouldRun_ = false;

}

bool IPCBroker::SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    Vector<String> pargs;

    workerProcess_ = new IPCProcess(context_);

    transport_.OpenServer(workerProcess_->fd1());

    // copy args
    for (unsigned i = 0; i < args.Size(); i++)
        pargs.Push(args[i]);

#ifdef ATOMIC_PLATFORM_WINDOWS
    // fd2 with be a HANDLE on windows
    /*
        wchar_t pipe_num[10];
        _i64tow_s(reinterpret_cast<__int64>(pp.fd2()), pipe_num, sizeof(pipe_num)/sizeof(pipe_num[0]), 10);
        writable_cmdline += kCmdLinePipeEq + std::wstring(pipe_num);
    */
#else
    pargs.Push(ToString("--ipc-server=%i", workerProcess_->fd1()));
    pargs.Push(ToString("--ipc-client=%i", workerProcess_->fd2()));
#endif

    if (!workerProcess_->Launch(command, pargs, initialDirectory))
        return false;

    close(workerProcess_->fd2());

    return Run();

}


}
