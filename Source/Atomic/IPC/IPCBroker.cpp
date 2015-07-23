
#ifndef ATOMIC_PLATFORM_WINDOWS
#include <unistd.h>
#endif

#include "../Core/StringUtils.h"
#include "../IO/Log.h"

#include "IPCUnix.h"
#include "IPCBroker.h"


namespace Atomic
{

unsigned IPCBroker::idCounter_ = 1;

IPCBroker::IPCBroker(Context* context) : IPCChannel(context, idCounter_)
{    
    idCounter_++;
}

IPCBroker::~IPCBroker()
{

}

void IPCBroker::ThreadFunction()
{
    while (shouldRun_)
    {
        if (!otherProcess_->IsRunning())
        {
            break;
        }

        if (!Receive())
        {            
            break;
        }
    }

    shouldRun_ = false;

}

bool IPCBroker::Update()
{

    if (otherProcess_.Null())
        return false;

    if (!shouldRun_)
    {
        Stop();
#ifndef ATOMIC_PLATFORM_WINDOWS
        close(pp_.fd1());
#endif
        return false;
    }

    return true;
}

bool IPCBroker::SpawnWorker(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    Vector<String> pargs;

    otherProcess_ = new IPCProcess(context_, pp_.clientRead(), pp_.clientWrite());

    transport_.OpenServer(pp_.serverRead(), pp_.serverWrite());

    // copy args
    for (unsigned i = 0; i < args.Size(); i++)
        pargs.Push(args[i]);

#ifdef ATOMIC_PLATFORM_WINDOWS

    wchar_t pipe_num[10];
    _i64tow_s(reinterpret_cast<__int64>(pp_.clientWrite()), pipe_num, sizeof(pipe_num)/sizeof(pipe_num[0]), 10);
    
    String cpipe;
    cpipe.SetUTF8FromWChar(pipe_num);

    String ipc_client = "--ipc-client=" + cpipe;
    pargs.Push(ipc_client);

    _i64tow_s(reinterpret_cast<__int64>(pp_.clientRead()), pipe_num, sizeof(pipe_num)/sizeof(pipe_num[0]), 10);

    String spipe;
    spipe.SetUTF8FromWChar(pipe_num);

    String ipc_server = "--ipc-server=" + spipe;
    pargs.Push(ipc_server);

#else
    pargs.Push(ToString("--ipc-server=%i", pp_.fd1()));
    pargs.Push(ToString("--ipc-client=%i", pp_.fd2()));    
#endif

    pargs.Push(ToString("--ipc-id=%i", id_));

    if (!otherProcess_->Launch(command, pargs, initialDirectory))
        return false;

#ifndef ATOMIC_PLATFORM_WINDOWS
    close(pp_.fd2());
#endif

    return Run();

}


}
