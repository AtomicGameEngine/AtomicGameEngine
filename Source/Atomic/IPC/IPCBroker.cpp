
#include "../Core/StringUtils.h"
#include "../IO/Log.h"

#include "IPCBroker.h"

#include <SimpleIPC/ipc_channel.h>
#include <SimpleIPC/ipc_codec.h>
#include <SimpleIPC/ipc_msg_dispatch.h>

typedef ipc::Channel<PipeTransport, ipc::Encoder, ipc::Decoder> PipeChannel;

namespace Atomic
{

IPCBroker::IPCBroker(Context* context) : Object(context),
    pp_(true),
    workerHandle_(0)
{

}

IPCBroker::~IPCBroker()
{

}

void IPCBroker::ThreadFunction()
{
    //PipeChannel channel(&transport);

    while (shouldRun_)
    {
        int answer;
        size_t sz = sizeof(int);
        transport_.Read(&answer, &sz);
        if (sz == sizeof(int))
        {
            shouldRun_ = false;
        }
    }

}

bool IPCBroker::SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    Poco::Process::Env env;
    return SpawnWorker(command, args, initialDirectory, env);
}

bool IPCBroker::SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory, const Poco::Process::Env& env)
{

    if (!transport_.OpenServer(pp_.fd1()))
    {
        return false;
    }

    Poco::Process::Args pargs;

    for (unsigned i = 0; i < args.Size(); i++)
        pargs.push_back(args[i].CString());

#ifdef ATOMIC_PLATFORM_WINDOWS
    // fd2 with be a HANDLE on windows
    /*
        wchar_t pipe_num[10];
        _i64tow_s(reinterpret_cast<__int64>(pp.fd2()), pipe_num, sizeof(pipe_num)/sizeof(pipe_num[0]), 10);
        writable_cmdline += kCmdLinePipeEq + std::wstring(pipe_num);
    */
#else
    String ipc = ToString("--ipc-server=%i", pp_.fd1());
    pargs.push_back(ipc.CString());
    ipc = ToString("--ipc-client=%i", pp_.fd2());
    pargs.push_back(ipc.CString());
#endif

    // socket pair only works same binary, have AtomicEditor be able to double as player?
    // this could be cool, as AtomicEditor could have some machinery that is left out of
    // the deployment player, and could simplify the cli (relaunch editor in player mode)


    std::string pcommand = command.CString();
    std::string pinitialDirectory = initialDirectory.CString();

    workerHandle_ = new Poco::ProcessHandle(Poco::Process::launch(pcommand, pargs, pinitialDirectory, NULL, NULL, NULL, env));

    if (!Poco::Process::isRunning(*workerHandle_))
        return false;

    close(pp_.fd2());

    return Run();

}


}
