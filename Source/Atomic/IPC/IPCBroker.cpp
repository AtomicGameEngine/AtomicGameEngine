
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
    unsigned count = 0;

    IPCMessageHeader header;
    header.messageType_ = IPC_MESSAGE_UNDEFINED;
    VectorBuffer dataBuffer;

    while (shouldRun_)
    {
        size_t sz = 0;

        const char* data = transport_.Receive(&sz);

        if (!data)
        {
            shouldRun_ = false;
            break;
        }

        if (!sz)
            continue;

        dataBuffer.Seek(dataBuffer.GetSize());
        dataBuffer.Write(data, sz);
        dataBuffer.Seek(0);

        while (true)
        {
            if (header.messageType_ == IPC_MESSAGE_UNDEFINED &&
                    dataBuffer.GetSize() - dataBuffer.GetPosition() >= sizeof(IPCMessageHeader))
            {
                dataBuffer.Read(&header, sizeof(IPCMessageHeader));
            }

            if (header.messageType_ == IPC_MESSAGE_UNDEFINED)
                break;

            if (header.messageType_ != IPC_MESSAGE_UNDEFINED &&
                     header.messageSize_ <= dataBuffer.GetSize() - dataBuffer.GetPosition())
            {
                MemoryBuffer buffer(dataBuffer.GetData() + dataBuffer.GetPosition(), header.messageSize_);
                dataBuffer.Seek( dataBuffer.GetPosition() + header.messageSize_);
                header.messageType_ = IPC_MESSAGE_UNDEFINED;

                IPCMessageEvent event;
                StringHash eventType;
                VariantMap eventData;
                event.DoRead(buffer, eventType, eventData);

                // LOGINFOF("Message: %s %i", eventData[eventType].ToString().CString(), (int) count++);
            }

            if (dataBuffer.IsEof())
            {
                dataBuffer.Clear();
            }

            if (dataBuffer.GetPosition() == 0)
                break;

            VectorBuffer newBuffer;
            newBuffer.Write(dataBuffer.GetData() + dataBuffer.GetPosition(), dataBuffer.GetSize() - dataBuffer.GetPosition());
            newBuffer.Seek(0);
            dataBuffer = newBuffer;
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
