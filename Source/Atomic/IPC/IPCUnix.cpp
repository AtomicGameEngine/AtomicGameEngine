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

#include "IPCUnix.h"

#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <errno.h>

#ifdef ATOMIC_PLATFORM_OSX
#include <libproc.h>
#endif

#ifdef ATOMIC_PLATFORM_LINUX
#include <sys/wait.h>
#endif

namespace Atomic
{

#define HANDLE_EINTR(x) ({ \
    typeof(x) __eintr_result__; \
    do { \
    __eintr_result__ = x; \
} while (__eintr_result__ == -1 && errno == EINTR); \
    __eintr_result__;\
})

bool SilenceSocket(int fd) {

#ifdef ATOMIC_PLATFORM_OSX
    int nosigpipe = 1;
    // On OSX an attempt to read or write to a closed socket may generate a
    // SIGPIPE rather than returning -1.  setsockopt will shut this off.
    if (0 != setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE,
                        &nosigpipe, sizeof nosigpipe)) {
        return false;
    }
#endif
    return true;
}

size_t ReadFromFD(int fd, char* buffer, size_t bytes) {

    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(fd, &set);

    timeout.tv_sec = 0;
    // 100ms
    timeout.tv_usec = 100000;

    // check if there is anything to read
    int rv = select(fd + 1, &set, NULL, NULL, &timeout);

    if (rv < 0)
        return -1;

    if (!rv)
        return 0;

    ssize_t bytes_read =
            HANDLE_EINTR(read(fd, buffer, bytes));
    if (bytes_read < 0) {
        return -1;
    }
    return bytes_read;
}

size_t WriteToFD(int fd, const char* data, size_t size) {
    // Allow for partial writes.
    ssize_t written_total = 0;
    for (ssize_t written_partial = 0; written_total < size; written_total += written_partial) {
        written_partial =
                HANDLE_EINTR(write(fd, data + written_total, size - written_total));
        if (written_partial < 0) {
            return -1;
        }
    }
    return written_total;
}

PipePair::PipePair() {
    fd_[0] = -1;
    fd_[1] = -1;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd_) !=0) {
        return;
    }
};

PipeUnix::PipeUnix() : fd_(-1) {
}

bool PipeUnix::OpenClient(int fd) {
    if (!SilenceSocket(fd)) {
        return false;
    }
    fd_ = fd;
    return true;
}

bool PipeUnix::OpenServer(int fd) {
    if (!SilenceSocket(fd)) {
        return false;
    }
    fd_ = fd;
    return true;
}


bool PipeUnix::Write(const void* buf, size_t sz) {
    if (sz == -1) {
        return false;
    }
    size_t written = WriteToFD(fd_, static_cast<const char*> (buf), sz);
    return (sz == written);
}

bool PipeUnix::Read(void* buf, size_t* sz) {
    size_t read = ReadFromFD(fd_, static_cast<char*> (buf), *sz);
    if (read == -1) {
        return false;
    }
    *sz = read;
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


IPCProcess::IPCProcess(Context* context, int fd1, int fd2, int pid) : Object(context),
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
    if (pid_ == -1)
        return false;

#if defined(ATOMIC_PLATFORM_OSX)
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    int ret = proc_pidpath (pid_, pathbuf, sizeof(pathbuf));
    if ( ret > 0 )
    {
        return true;
    }
#elif defined(ATOMIC_PLATFORM_LINUX)
    int status;
    pid_t childPid = waitpid( pid_, &status, WNOHANG );
    bool childRunning = !WIFEXITED( status ) && !WIFSIGNALED( status ) && !WIFSTOPPED( status );
    if ( childPid != pid_ || childRunning )
    {
        return true;
    }
#else
    // this doesn't seem to work on OSX?
    if (kill(pid_, 0) == 0)
        return true;
#endif

    return false;

}

bool IPCProcess::Launch(const String& command, const Vector<String>& args, const String& initialDirectory)
{

    assert(pid_ == -1);

    // We must not allocated memory after fork(),
    // therefore allocate all required buffers first.

    PODVector<char*> argv;
    argv.Resize(args.Size() + 2);
    int i = 0;
    argv[i++] = const_cast<char*>(command.CString());
    for (unsigned j = 0; j < args.Size(); j++)
    {
        argv[i++] = const_cast<char*>(args[j].CString());
    }
    argv[i] = NULL;

    const char* pInitialDirectory = initialDirectory.Empty() ? 0 : initialDirectory.CString();

    int pid = fork();
    if (pid < 0)
    {
        // error forking
        return false;
    }
    else if (pid == 0)
    {
        // child process
        if (pInitialDirectory)
        {
            if (chdir(pInitialDirectory) != 0)
            {
                _exit(72);
            }
        }

        // close all open file descriptors other than stdin, stdout, stderr
        // and the IPC child fd
        for (int i = 3; i < getdtablesize(); ++i)
        {
            if (i != fd2())
                close(i);
        }

        execvp(argv[0], &argv[0]);
        _exit(72);

    }
    else
    {
        // parent process
        pid_ = pid;
    }

    return true;
}

}


#endif
