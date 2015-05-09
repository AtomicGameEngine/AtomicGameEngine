#ifdef ATOMIC_PLATFORM_WINDOWS

#include "IPCWindows.h"

namespace Atomic
{

PipePair::PipePair(bool inherit_fd2)
{
    srv_ = INVALID_IPCHANDLE_VALUE;
    cln_ = INVALID_IPCHANDLE_VALUE;

}

PipeWin::PipeWin() : pipe_(INVALID_IPCHANDLE_VALUE) {
}

PipeWin::~PipeWin()
{

}

bool PipeWin::OpenClient(IPCHandle pipe) {
    return true;
}

bool PipeWin::OpenServer(IPCHandle pipe, bool connect) {
    return true;
}


bool PipeWin::Write(const void* buf, size_t sz)
{
    return true;
}

bool PipeWin::Read(void* buf, size_t* sz) {
    return true;
}


char* PipeTransport::Receive(size_t* size) {
    if (buf_.Size() < kBufferSz) {
        buf_.Resize(kBufferSz);
    }

    *size = kBufferSz;
    if (!Read(&buf_[0], size)) {
        return NULL;
    }
    return &buf_[0];
}


IPCProcess::IPCProcess(Context* context, IPCHandle fd1, IPCHandle fd2, IPCHandle pid) : Object(context),
    pid_(pid),
    fd1_(fd1),
    fd2_(fd2)
{
}

IPCProcess::~IPCProcess()
{

}

bool IPCProcess::IsRunning()
{
    return false;

}

bool IPCProcess::Launch(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    return false;
}

}

#endif
