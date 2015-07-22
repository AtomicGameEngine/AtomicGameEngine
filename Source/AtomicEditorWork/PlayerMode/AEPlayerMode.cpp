
#include <Atomic/IO/Log.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCWorker.h>

#include "AEPlayerMode.h"

namespace AtomicEditor
{

PlayerMode::PlayerMode(Context* context) :
    Object(context)
{
    fd_[0] = INVALID_IPCHANDLE_VALUE;
    fd_[1] = INVALID_IPCHANDLE_VALUE;
}

void PlayerMode::HandleHelloFromBroker(StringHash eventType, VariantMap& eventData)
{
    assert(eventData[HelloFromBroker::P_HELLO].GetString() == "Hello");
    assert(eventData[HelloFromBroker::P_LIFETHEUNIVERSEANDEVERYTHING].GetInt() == 42);

    LOGERROR("Passed Test!");
}

void PlayerMode::ProcessArguments() {

    const Vector<String>& arguments = GetArguments();

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1)
        {
            String argument = arguments[i].ToLower();
            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            if (argument.StartsWith("--ipc-server=") || argument.StartsWith("--ipc-client="))
            {
                LOGINFOF("Starting IPCWorker %s", argument.CString());

                Vector<String> ipc = argument.Split(argument.CString(), '=');

                if (ipc.Size() == 2)
                {
                    if (argument.StartsWith("--ipc-server="))
                    {
#ifdef ATOMIC_PLATFORM_WINDOWS
                        WString wipc(ipc[1]);
                        HANDLE pipe = reinterpret_cast<HANDLE>(_wtoi64(wipc.CString()));
                        fd_[0] = pipe;
#else
                        int fd = ToInt(ipc[1].CString());
                        fd_[0] = fd;
#endif
                    }
                    else
                    {
#ifdef ATOMIC_PLATFORM_WINDOWS
                        WString wipc(ipc[1]);
                        HANDLE pipe = reinterpret_cast<HANDLE>(_wtoi64(wipc.CString()));
                        fd_[1] = pipe;
#else
                        int fd = ToInt(ipc[1].CString());
                        fd_[1] = fd;
#endif
                    }

                }

            }

        }
    }

#ifdef ATOMIC_PLATFORM_WINDOWS
    if (fd_[0] != INVALID_IPCHANDLE_VALUE)
    {
        //::CloseHandle(fd_[0]);
        fd_[0] = INVALID_IPCHANDLE_VALUE;
    }

    if (fd_[1] != INVALID_IPCHANDLE_VALUE)
    {
        IPC* ipc = new IPC(context_);
        context_->RegisterSubsystem(ipc);
        //ipc->InitWorker(fd_[0], fd_[1]);
    }
#else
    if (fd_[0] != INVALID_IPCHANDLE_VALUE && fd_[1] != INVALID_IPCHANDLE_VALUE)
    {
        IPC* ipc = GetSubsystem<IPC>();
        SubscribeToEvent(E_IPCHELLOFROMBROKER, HANDLER(PlayerMode, HandleHelloFromBroker));
        ipc->InitWorker(fd_[0], fd_[1]);
    }
#endif

}

PlayerMode::~PlayerMode()
{

}

}
