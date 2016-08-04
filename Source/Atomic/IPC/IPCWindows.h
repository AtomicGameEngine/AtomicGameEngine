//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifdef ATOMIC_PLATFORM_WINDOWS

#pragma once

#include "../Core/Mutex.h"
#include "../Core/Thread.h"
#include "../Core/Object.h"
#include "IPCTypes.h"

namespace Atomic
{

#define ATOMIC_WINDOWS_IPC_BUFFER_SIZE 1048576

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

    class ReaderThread : public Thread
    {
    public:

        ReaderThread(PipeWin* pipeWin) : pipeWin_(pipeWin), readSize_(0)
        {
            buf_.Resize(ATOMIC_WINDOWS_IPC_BUFFER_SIZE);
        }

        void Kill();
        void ThreadFunction();

        Mutex mutex_;
        PipeWin* pipeWin_;
        PODVector<char> buf_;
        unsigned readSize_;
    };

    IPCHandle pipeRead_;
    IPCHandle pipeWrite_;

    ReaderThread readerThread_;

};


class PipeTransport : public PipeWin {
public:

    static const size_t kBufferSz = ATOMIC_WINDOWS_IPC_BUFFER_SIZE;

    bool Send(const void* buf, size_t sz) {
        return Write(buf, sz);
    }

    char* Receive(size_t* size);

private:
    PODVector<char> buf_;
};

class IPCProcess : public Object
{
    ATOMIC_OBJECT(IPCProcess, Object)

    public:

    IPCProcess(Context* context, IPCHandle clientRead, IPCHandle clientWrite, IPCHandle pid = INVALID_IPCHANDLE_VALUE);

    virtual ~IPCProcess();

    bool IsRunning();

    bool Terminate();

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
