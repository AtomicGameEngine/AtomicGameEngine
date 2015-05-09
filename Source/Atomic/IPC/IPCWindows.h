
#ifdef ATOMIC_PLATFORM_WINDOWS

#pragma once

#include "../Core/Object.h"
#include "IPCTypes.h"

namespace Atomic
{

class PipePair {

public:
    PipePair(bool inherit_fd2 = false);
    IPCHandle fd1() const { return srv_; }
    IPCHandle fd2() const { return cln_; }

    static IPCHandle OpenPipeServer(const wchar_t* name, bool low_integrity = true);
    static IPCHandle OpenPipeClient(const wchar_t* name, bool inherit, bool impersonate);

private:
    IPCHandle srv_;
    IPCHandle cln_;
};

class PipeWin {
public:
    PipeWin();
    ~PipeWin();

    bool OpenClient(IPCHandle pipe);
    bool OpenServer(IPCHandle pipe, bool connect = false);

    bool Write(const void* buf, size_t sz);
    bool Read(void* buf, size_t* sz);

    bool IsConnected() const { return pipe_ != INVALID_IPCHANDLE_VALUE; }

private:
    IPCHandle pipe_;
};


class PipeTransport : public PipeWin {
public:
    static const size_t kBufferSz = 4096;

    bool Send(const void* buf, size_t sz) {
        return Write(buf, sz);
    }

    char* Receive(size_t* size);

private:
    PODVector<char> buf_;
};

class IPCProcess : public Object
{
    OBJECT(IPCProcess)

    public:

    IPCProcess(Context* context, IPCHandle fd1, IPCHandle fd2, IPCHandle pid = INVALID_IPCHANDLE_VALUE);

    virtual ~IPCProcess();

    bool IsRunning();

    IPCHandle fd1() const { return fd1_; }
    IPCHandle fd2() const { return fd2_; }

    bool Launch(const String& command, const Vector<String>& args, const String& initialDirectory);

private:

    IPCHandle pid_;
    IPCHandle fd1_;
    IPCHandle fd2_;
};

}

#endif
