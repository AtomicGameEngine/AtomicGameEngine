
#include <unistd.h>

#include "../Core/StringUtils.h"
#include "../IO/Log.h"

#include "IPCUnix.h"
#include "IPCBroker.h"
#include "IPCMessage.h"

namespace Atomic
{

IPCBroker::IPCBroker(Context* context) : Object(context)
{

}

IPCBroker::~IPCBroker()
{

}

void IPCBroker::ThreadFunction()
{
    while (shouldRun_)
    {
        unsigned msgType;
        unsigned msgSize;

        size_t sz = sizeof(unsigned);
        transport_.Read(&msgType, &sz);

        sz = sizeof(unsigned);
        transport_.Read(&msgSize, &sz);

        VectorBuffer buffer;
        buffer.Resize(msgSize);

        sz = msgSize;
        transport_.Read(buffer.GetModifiableData(), &sz);
        assert(sz = msgSize);

        IPCMessageEvent event;

        StringHash eventType;
        VariantMap eventData;
        event.DoRead(buffer, eventType, eventData);
    }

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
