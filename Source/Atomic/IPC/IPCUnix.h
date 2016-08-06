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

#ifndef ATOMIC_PLATFORM_WINDOWS

#pragma once

#include "../Core/Object.h"

namespace Atomic
{

class PipePair {
public:
    PipePair();

    int fd1() const { return fd_[0]; }
    int fd2() const { return fd_[1]; }

private:
    int fd_[2];
};


class PipeUnix {
public:
    PipeUnix();

    bool OpenClient(int fd);
    bool OpenServer(int fd);

    bool Write(const void* buf, size_t sz);
    bool Read(void* buf, size_t* sz);

    bool IsConnected() const { return fd_ != -1; }

private:
    int fd_;
};


class PipeTransport : public PipeUnix {
public:
    static const size_t kBufferSz = 1024 * 1024;

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

    IPCProcess(Context* context, int fd1, int fd2, int pid = -1);

    virtual ~IPCProcess();

    bool IsRunning();

    bool Terminate() { return true; }

    int fd1() const { return fd1_; }
    int fd2() const { return fd2_; }

    bool Launch(const String& command, const Vector<String>& args, const String& initialDirectory);

private:

    int pid_;
    int fd1_;
    int fd2_;
};


}


#endif
