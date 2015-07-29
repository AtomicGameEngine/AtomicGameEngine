
#ifdef ATOMIC_PLATFORM_WINDOWS

#pragma once

#include "../Core/Object.h"
#include "IPCTypes.h"

namespace Atomic
{

class PipePair {

public:
    PipePair();

    IPCHandle serverRead() const { return srvRead_; }
    IPCHandle serverWrite() const { return srvWrite_; }

	IPCHandle clientRead() const { return clnRead_; }
	IPCHandle clientWrite() const { return clnWrite_; }

    static IPCHandle OpenPipeServer(const wchar_t* name, bool read);
    static IPCHandle OpenPipeClient(const wchar_t* name, bool read);

private:
    IPCHandle srvRead_;
	IPCHandle srvWrite_;

	IPCHandle clnRead_;
	IPCHandle clnWrite_;
};

class PipeWin {
public:
    PipeWin();
    ~PipeWin();

    bool OpenClient(IPCHandle pipeRead, IPCHandle pipeWrite);
    bool OpenServer(IPCHandle pipeRead, IPCHandle pipeWrite);

    bool Write(const void* buf, size_t sz);
    bool Read(void* buf, size_t* sz);

    bool IsConnected() const { return pipeRead_ != INVALID_IPCHANDLE_VALUE && pipeWrite_ != INVALID_IPCHANDLE_VALUE; }

private:
    IPCHandle pipeRead_;
	IPCHandle pipeWrite_;
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

    IPCProcess(Context* context, IPCHandle clientRead, IPCHandle clientWrite, IPCHandle pid = INVALID_IPCHANDLE_VALUE);

    virtual ~IPCProcess();

    bool IsRunning();

    IPCHandle clientRead() const { return clientRead_; }
    IPCHandle clientWrite() const { return clientWrite_; }

    bool Launch(const String& command, const Vector<String>& args, const String& initialDirectory);

private:

    IPCHandle pid_;
    IPCHandle clientRead_;
    IPCHandle clientWrite_;
};

}

#endif
