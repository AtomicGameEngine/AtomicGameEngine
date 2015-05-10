#ifdef ATOMIC_PLATFORM_WINDOWS

#include <Windows.h>
#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#include <string>

#include "IPCWindows.h"

typedef std::wstring IPCWString;

namespace Atomic
{

    static const wchar_t kPipePrefix[] = L"\\\\.\\pipe\\";
    static const int kPipeBufferSz = 4 * 1024;
    static LONG g_pipe_seq = 0;

    bool checkIntegritySupport()
    {
        OSVERSIONINFO osvi;

        ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        GetVersionEx(&osvi);

        return osvi.dwMajorVersion > 5;
    }

    HANDLE PipePair::OpenPipeServer(const wchar_t* name, bool low_integrity) {
        SECURITY_ATTRIBUTES sa = { 0 };
        SECURITY_ATTRIBUTES *psa = 0;

        static const bool is_integrity_supported = false;// checkIntegritySupport();

        if (is_integrity_supported && low_integrity) {
            psa = &sa;
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.bInheritHandle = TRUE;
            if (!ConvertStringSecurityDescriptorToSecurityDescriptor(
                TEXT("S:(ML;;NWNR;;;LW)"), SDDL_REVISION_1, &sa.lpSecurityDescriptor, NULL))
                return INVALID_HANDLE_VALUE;
        }

        IPCWString pipename(kPipePrefix);
        pipename.append(name);
        return ::CreateNamedPipeW(pipename.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1, kPipeBufferSz, kPipeBufferSz, 200, psa);
    }

    HANDLE PipePair::OpenPipeClient(const wchar_t* name, bool inherit, bool impersonate) {
        IPCWString pipename(kPipePrefix);
        pipename.append(name);

        SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, inherit ? TRUE : FALSE };
        for (;;) {
            DWORD attributes = impersonate ? 0 : SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION;
            HANDLE pipe = ::CreateFileW(pipename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, &sa,
                OPEN_EXISTING, attributes, NULL);

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

    PipePair::PipePair(bool inherit_fd2) :
        srv_(INVALID_IPCHANDLE_VALUE),
        cln_(INVALID_IPCHANDLE_VALUE)
    {
        // Come up with a reasonable unique name.
        const wchar_t kPipePattern[] = L"ko.%x.%x.%x";
        
        wchar_t name[8 * 3 + sizeof(kPipePattern)];
        
        ::wsprintfW(name, kPipePattern, ::GetCurrentProcessId(), ::GetTickCount(),
            ::InterlockedIncrement(&g_pipe_seq));
        
        HANDLE server = OpenPipeServer(name);
        
        if (INVALID_HANDLE_VALUE == server) 
        {
            return;
        }
        
        // Don't allow client impersonation.
        HANDLE client = OpenPipeClient(name, inherit_fd2, false);

        if (INVALID_HANDLE_VALUE == client) 
        {
            ::CloseHandle(server);
            return;
        }
        if (!::ConnectNamedPipe(server, NULL)) 
        {
            if (ERROR_PIPE_CONNECTED != ::GetLastError()) 
            {
                ::CloseHandle(server);
                ::CloseHandle(client);
                return;
            }
        }

        srv_ = server;
        cln_ = client;

    }

    PipeWin::PipeWin() : pipe_(INVALID_IPCHANDLE_VALUE)
    {

    }

    PipeWin::~PipeWin()
    {
        if (pipe_ != INVALID_HANDLE_VALUE) 
        {
            ::DisconnectNamedPipe(pipe_);  // $$$ disconect is valid on the server side.
            ::CloseHandle(pipe_);
        }
    }

    bool PipeWin::OpenClient(IPCHandle pipe)
    {
        pipe_ = pipe;
        return true;

    }

    bool PipeWin::OpenServer(IPCHandle pipe, bool connect)
    {
        pipe_ = pipe;

        if (connect) 
        {
            if (!::ConnectNamedPipe(pipe, NULL)) 
            {
                if (ERROR_PIPE_CONNECTED != ::GetLastError()) 
                {
                    return false;
                }
            }
        }

        return true;
    }


    bool PipeWin::Write(const void* buf, size_t sz)
    {
        DWORD written = 0;
        if (TRUE == ::WriteFile(pipe_, buf, sz, &written, NULL))
            return true;

        return false;
    }

    bool PipeWin::Read(void* buf, size_t* sz)
    {
        if (TRUE == ::ReadFile(pipe_, buf, *sz, reinterpret_cast<DWORD*>(sz), NULL))
        {
            return true;
        }

        return false;
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
        return true;

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

        pid_ = pi.hProcess;
        ::CloseHandle(pi.hThread);
        
        // The client side of the pipe has been already duplicated into the worker process.
        ::CloseHandle(fd2_);

        return true;
    }

}

#endif
