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

    PipeWin::PipeWin() : pipeRead_(INVALID_IPCHANDLE_VALUE), pipeWrite_(INVALID_IPCHANDLE_VALUE)
    {

    }

    PipeWin::~PipeWin()
    {
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
        return true;

    }

    bool PipeWin::OpenServer(IPCHandle pipeRead, IPCHandle pipeWrite)
    {
		pipeRead_ = pipeRead;
		pipeWrite_ = pipeWrite;
		return true;
	}


    bool PipeWin::Write(const void* buf, size_t sz)
    {
        DWORD written = 0;
        if (TRUE == ::WriteFile(pipeWrite_, buf, sz, &written, NULL))
            return true;

        return false;
    }

    bool PipeWin::Read(void* buf, size_t* sz)
    {
        if (TRUE == ::ReadFile(pipeRead_, buf, *sz, reinterpret_cast<DWORD*>(sz), NULL))
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


    IPCProcess::IPCProcess(Context* context, IPCHandle clientRead, IPCHandle clientWrite, IPCHandle pid) : Object(context),
        pid_(pid),
        clientRead_(clientRead),
        clientWrite_(clientWrite)
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
        
        return true;
    }

}

#endif
