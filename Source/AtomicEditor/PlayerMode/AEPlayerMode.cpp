
#include <Atomic/IO/IOEvents.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/UI/SystemUI/DebugHud.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCWorker.h>

#include <AtomicJS/Javascript/JSVM.h>
#include <AtomicJS/Javascript/JSEvents.h>
#include <AtomicJS/Javascript/JSIPCEvents.h>

#include "AEPlayerMode.h"

namespace AtomicEditor
{

PlayerMode::PlayerMode(Context* context) :
    Object(context),
    brokerActive_(false),
    launchedByEditor_(false)
{
    fd_[0] = INVALID_IPCHANDLE_VALUE;
    fd_[1] = INVALID_IPCHANDLE_VALUE;

    ipc_ = GetSubsystem<IPC>();

    SubscribeToEvent(E_LOGMESSAGE, HANDLER(PlayerMode, HandleLogMessage));
    SubscribeToEvent(E_JSERROR, HANDLER(PlayerMode, HandleJSError));
}

PlayerMode::~PlayerMode()
{

}

void PlayerMode::HandleIPCInitialize(StringHash eventType, VariantMap& eventData)
{
    brokerActive_ = true;

    JSVM* vm = JSVM::GetJSVM(0);

    if (!vm->ExecuteMain())
    {
        SendEvent(E_EXITREQUESTED);
    }

    SystemUI::DebugHud* debugHud = GetSubsystem<SystemUI::DebugHud>();
    if (debugHud)
        debugHud->SetMode(eventData["debugHudMode"].GetUInt());

}

void PlayerMode::ProcessArguments() {

    const Vector<String>& arguments = GetArguments();

    int id = -1;

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1)
        {
            String argument = arguments[i].ToLower();
            // String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            if (argument.StartsWith("--ipc-id="))
            {
                Vector<String> idc = argument.Split(argument.CString(), '=');
                if (idc.Size() == 2)

                id = ToInt(idc[1].CString());
            }

            else if (argument.StartsWith("--ipc-server=") || argument.StartsWith("--ipc-client="))
            {
                LOGINFOF("Starting IPCWorker %s", argument.CString());

                Vector<String> ipc = argument.Split(argument.CString(), '=');

                if (ipc.Size() == 2)
                {
                    if (argument.StartsWith("--ipc-server="))
                    {
#ifdef ATOMIC_PLATFORM_WINDOWS
						// clientRead
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
						// clientWrite
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

    if (id > 0 && fd_[0] != INVALID_IPCHANDLE_VALUE && fd_[1] != INVALID_IPCHANDLE_VALUE)
    {        
        launchedByEditor_ = true;
        SubscribeToEvent(E_IPCINITIALIZE, HANDLER(PlayerMode, HandleIPCInitialize));
        ipc_->InitWorker((unsigned) id, fd_[0], fd_[1]);
    }
}

void PlayerMode::HandleJSError(StringHash eventType, VariantMap& eventData)
{
    if (brokerActive_)
    {
        if (ipc_.Null())
            return;

        String errName = eventData[JSError::P_ERRORNAME].GetString();
        String errStack = eventData[JSError::P_ERRORSTACK].GetString();
        String errMessage = eventData[JSError::P_ERRORMESSAGE].GetString();
        String errFilename = eventData[JSError::P_ERRORFILENAME].GetString();
        int errLineNumber = eventData[JSError::P_ERRORLINENUMBER].GetInt();

        VariantMap ipcErrorData;
        ipcErrorData[IPCJSError::P_ERRORNAME] = errName;
        ipcErrorData[IPCJSError::P_ERRORSTACK] = errStack;
        ipcErrorData[IPCJSError::P_ERRORMESSAGE] = errMessage;
        ipcErrorData[IPCJSError::P_ERRORFILENAME] = errFilename;
        ipcErrorData[IPCJSError::P_ERRORLINENUMBER] = errLineNumber;

        ipc_->SendEventToBroker(E_IPCJSERROR, ipcErrorData);

        LOGERROR("SENDING E_IPCJSERROR");

    }

}


void PlayerMode::HandleLogMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace LogMessage;

    if (brokerActive_)
    {

        if (ipc_.Null())
            return;

        VariantMap logEvent;
        logEvent[IPCWorkerLog::P_LEVEL] = eventData[P_LEVEL].GetInt();
        logEvent[IPCWorkerLog::P_MESSAGE] = eventData[P_MESSAGE].GetString();
        ipc_->SendEventToBroker(E_IPCWORKERLOG, logEvent);
    }

}


}
