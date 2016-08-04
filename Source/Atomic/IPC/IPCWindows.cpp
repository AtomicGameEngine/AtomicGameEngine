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

#include <Windows.h>
#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#include <string>

#include "../Core/Timer.h"
#include "../IO/Log.h"
#include "IPCWindows.h"

#include "IPC.h"

typedef std::wstring IPCWString;

namespace Atomic
{

static const wchar_t kPipePrefix[] = L"\\\\.\\pipe\\";
// start with 1 megabyte of buffer, this will grow if a request exceeds this size
// however, it will block during resize
static const int kPipeBufferSz = ATOMIC_WINDOWS_IPC_BUFFER_SIZE;
static LONG g_pipe_seq = 0;

HANDLE PipePair::OpenPipeServer(const wchar_t* name, bool read)
{
    IPCWString pipename(kPipePrefix);
    pipename.append(name);

    DWORD openMode = read ? PIPE_ACCESS_INBOUND : PIPE_ACCESS_OUTBOUND;

    return ::CreateNamedPipeW(pipename.c_str(), openMode,
                              PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                              1, kPipeBufferSz, kPipeBufferSz, 200, NULL);
}

HANDLE PipePair::OpenPipeClient(const wchar_t* name, bool read)
{
    IPCWString pipename(kPipePrefix);
    pipename.append(name);

    SECURITY_ATTRIBUTES sa;
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);

    DWORD accessMode = read ? GENERIC_READ : GENERIC_WRITE;

    for (;;) {

        HANDLE pipe = ::CreateFileW(pipename.c_str(), accessMode, 0, &sa,
                                    OPEN_EXISTING, 0, NULL);

        if (INVALID_HANDLE_VALUE == pipe) {
            if (ERROR_PIPE_BUSY != ::GetLastError()) {
                return pipe;
            }
            // wait and retry.
            ::Sleep(25);
        }
        else {
            // success.
            return pipe;
        }
    }
}

PipePair::PipePair() :
    srvRead_(INVALID_IPCHANDLE_VALUE),
    srvWrite_(INVALID_IPCHANDLE_VALUE),
    clnRead_(INVALID_IPCHANDLE_VALUE),
    clnWrite_(INVALID_IPCHANDLE_VALUE)
{
    // Come up with a reasonable unique name.
    const wchar_t kPipePattern[] = L"ko.%x.%x.%x";

    wchar_t serverReadName[8 * 3 + sizeof(kPipePattern)];
    ::wsprintfW(serverReadName, kPipePattern, ::GetCurrentProcessId(), ::GetTickCount(),
                ::InterlockedIncrement(&g_pipe_seq));

    wchar_t serverWriteName[8 * 3 + sizeof(kPipePattern)];
    ::wsprintfW(serverWriteName, kPipePattern, ::GetCurrentProcessId(), ::GetTickCount() + 1,
                ::InterlockedIncrement(&g_pipe_seq));

    srvRead_ = OpenPipeServer(serverReadName, true);
    srvWrite_ = OpenPipeServer(serverWriteName, false);


    // Don't allow client impersonation.
    clnRead_ = OpenPipeClient(serverWriteName, true);
    clnWrite_ = OpenPipeClient(serverReadName, false);

    /*
        if (INVALID_HANDLE_VALUE == client)
        {
            ::CloseHandle(server);
            return;
        }
        */

    if (!::ConnectNamedPipe(srvRead_, NULL))
    {
        if (ERROR_PIPE_CONNECTED != ::GetLastError())
        {
            // ::CloseHandle(server);
            //::CloseHandle(client);
            return;
        }
    }

    if (!::ConnectNamedPipe(srvWrite_, NULL))
    {
        if (ERROR_PIPE_CONNECTED != ::GetLastError())
        {
            // ::CloseHandle(server);
            //::CloseHandle(client);
            return;
        }
    }

}

PipeWin::PipeWin() : pipeRead_(INVALID_IPCHANDLE_VALUE), pipeWrite_(INVALID_IPCHANDLE_VALUE), readerThread_(this)
{

}

PipeWin::~PipeWin()
{
    readerThread_.Kill();

    if (pipeRead_ != INVALID_HANDLE_VALUE)
    {
        ::DisconnectNamedPipe(pipeRead_);  // $$$ disconect is valid on the server side.
        ::CloseHandle(pipeRead_);
    }

    if (pipeWrite_ != INVALID_HANDLE_VALUE)
    {
        ::DisconnectNamedPipe(pipeWrite_);  // $$$ disconect is valid on the server side.
        ::CloseHandle(pipeWrite_);
    }

}

bool PipeWin::OpenClient(IPCHandle pipeRead, IPCHandle pipeWrite)
{
    pipeRead_ = pipeRead;
    pipeWrite_ = pipeWrite;

    readerThread_.Run();

    return true;

}

bool PipeWin::OpenServer(IPCHandle pipeRead, IPCHandle pipeWrite)
{
    pipeRead_ = pipeRead;
    pipeWrite_ = pipeWrite;

    readerThread_.Run();

    return true;
}


bool PipeWin::Write(const void* buf, size_t sz)
{
    DWORD written = 0;
    if (TRUE == ::WriteFile(pipeWrite_, buf, (DWORD) sz, &written, NULL))
        return true;

    return false;
}

void PipeWin::ReaderThread::Kill()
{
    if (handle_)
    {
        BOOL result = TerminateThread((HANDLE)handle_, 0);
        result = CloseHandle((HANDLE)handle_);
        handle_ = 0;
    }
}

void PipeWin::ReaderThread::ThreadFunction()
{
    while(shouldRun_)
    {
        if (readSize_)
            continue;

        DWORD bytesRead = 0;
        if (TRUE == ::ReadFile(pipeWin_->pipeRead_, &buf_[0], ATOMIC_WINDOWS_IPC_BUFFER_SIZE, &bytesRead, NULL))
        {
            readSize_ = (unsigned) bytesRead;
        }

        Time::Sleep(10);

    }
}

bool PipeWin::Read(void* buf, size_t* sz)
{

    *sz = 0;
    if (readerThread_.readSize_)
    {
        memcpy(buf, &readerThread_.buf_[0], readerThread_.readSize_);
        *sz = readerThread_.readSize_;
        readerThread_.readSize_ = 0;
    }

    return true;
}

char* PipeTransport::Receive(size_t* size)
{
    if (buf_.Size() < kBufferSz)
    {
        buf_.Resize(kBufferSz);
    }

    *size = kBufferSz;

    if (!Read(&buf_[0], size))
    {
        return NULL;
    }
    return &buf_[0];
}


IPCProcess::IPCProcess(Context* context, IPCHandle clientRead, IPCHandle clientWrite, IPCHandle pid) : Object(context),
    pid_(pid),
    clientRead_(clientRead),
    clientWrite_(clientWrite)
{
}

IPCProcess::~IPCProcess()
{

}

bool IPCProcess::Terminate()
{
    if (TerminateProcess(pid_, 0))
    {
        WaitForSingleObject(pid_, 1000);
        return true;
    }
        
    return false;
}

bool IPCProcess::IsRunning()
{
    DWORD exitCode;
    if (!GetExitCodeProcess(pid_, &exitCode))
        return false;

    return exitCode == STILL_ACTIVE;
}

bool IPCProcess::Launch(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // CreateProcess wants a single string
    String sargs;
    sargs.Join(args, " ");

    // convert to wide
    WString wcommand(command);

    // prepend the command and convert to wide
    WString wargs("\"" + command + "\" " + sargs);

    // The child process inherits the pipe handle.
    if (!::CreateProcessW(wcommand.CString(), (LPWSTR) wargs.CString(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return false;
    }

    IPC* ipc = GetSubsystem<IPC>();
    IPCHandle jobHandle = ipc->GetJobHandle();

    if (jobHandle)
    {
        if (0 == AssignProcessToJobObject(jobHandle, pi.hProcess))
        {
            ATOMIC_LOGERROR("IPCProcess::Launch - unable to assign job");
        }
    }

    pid_ = pi.hProcess;
    ::CloseHandle(pi.hThread);

    return true;
}

}

#endif
