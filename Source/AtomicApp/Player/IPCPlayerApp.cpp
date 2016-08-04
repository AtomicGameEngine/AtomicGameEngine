//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/Core/CoreEvents.h>//
#include <Atomic/IO/IOEvents.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/GraphicsEvents.h>
#include <Atomic/IPC/IPCEvents.h>
#include <AtomicJS/Javascript/JSIPCEvents.h>
#include "IPCPlayerAppEvents.h"

#include <Atomic/Engine/Engine.h>
#include <Atomic/IPC/IPC.h>
#include <AtomicJS/Javascript/Javascript.h>
#include <Atomic/UI/SystemUI/DebugHud.h>

#include "IPCPlayerApp.h"

namespace Atomic
{

    IPCPlayerApp::IPCPlayerApp(Context* context) :
        PlayerApp(context),
        subprocess_(false),
        debugPlayer_(false),
        brokerActive_(false)
    {
        fd_[0] = INVALID_IPCHANDLE_VALUE;
        fd_[1] = INVALID_IPCHANDLE_VALUE;        
    }

    IPCPlayerApp::~IPCPlayerApp()
    {

    }

    void IPCPlayerApp::Setup()
    {
        PlayerApp::Setup();

        // This should be configurable
        engineParameters_.InsertNew("LogLevel", LOG_DEBUG);

        ipc_ = new IPC(context_);
        context_->RegisterSubsystem(ipc_);
    }

    void IPCPlayerApp::ProcessArguments()
    {
        PlayerApp::ProcessArguments();

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        if (!fileSystem)
        {
            ErrorExit("IPCPlayerApp::ProcessArguments FileSystem subsystem does not exist");
        }

        String resourcePrefix;
        engineParameters_["ResourcePrefixPaths"] = "";

        for (unsigned i = 0; i < arguments_.Size(); ++i)
        {
            if (arguments_[i].Length() > 1)
            {
                String argument = arguments_[i].ToLower();
                String value = i + 1 < arguments_.Size() ? arguments_[i + 1] : String::EMPTY;

                if (argument.StartsWith("--ipc-server=") || argument.StartsWith("--ipc-client="))
                {
                    subprocess_ = true;
                }
                else if (argument == "--debug")
                {
                    debugPlayer_ = true;
                }
                else if (argument == "--resourceprefix" && value.Length())
                {
                    resourcePrefix = value;
                    engineParameters_["ResourcePrefixPaths"] = resourcePrefix;
                }
                else if (argument == "--project" && value.Length())
                {
                    value = AddTrailingSlash(value);

                    AddEngineConfigSearchPath(value + "Settings/");

                    // check that cache exists
                    if (!fileSystem->DirExists(value + "Cache"))
                    {
                        ErrorExit("Project cache folder does not exist, projects must be loaded into the Atomic Editor at least once before using the --player command line mode");
                        return;
                    }

#ifdef ATOMIC_DEV_BUILD

                    String resourcePaths = ToString("%s/Resources/CoreData;%s/Resources/PlayerData;%sResources;%s;%sCache",
                        ATOMIC_ROOT_SOURCE_DIR, ATOMIC_ROOT_SOURCE_DIR, value.CString(), value.CString(), value.CString());

#else

#ifdef __APPLE__
                    engineParameters_["ResourcePrefixPaths"] = "../Resources";
#else
                    if (!resourcePrefix.Length())
                    {
                        engineParameters_["ResourcePrefixPaths"] = fileSystem->GetProgramDir() + "Resources";
                    }

#endif

                    String resourcePaths = ToString("CoreData;PlayerData;%s/;%s/Resources;%s;%sCache",
                        value.CString(), value.CString(), value.CString(), value.CString());
#endif

                    ATOMIC_LOGINFOF("Adding ResourcePaths: %s", resourcePaths.CString());

                    engineParameters_["ResourcePaths"] = resourcePaths;

                }
            }
        }

        // IPC client player should not auto exit if a subprocess
        if (subprocess_)
            engine_->SetAutoExit(false);

    }

    void IPCPlayerApp::Start()
    {

        if (subprocess_)
        {
            // do not execute main in the player app
            executeJSMain_ = false;
        }

        PlayerApp::Start();

        int id = -1;

        if (IPC::ProcessArguments(arguments_, id, fd_[0], fd_[1]))
        {
            SubscribeToEvent(E_IPCINITIALIZE, ATOMIC_HANDLER(IPCPlayerApp, HandleIPCInitialize));
            SubscribeToEvent(E_LOGMESSAGE, ATOMIC_HANDLER(IPCPlayerApp, HandleLogMessage));
            SubscribeToEvent(E_JSERROR, ATOMIC_HANDLER(IPCPlayerApp, HandleJSError));
            SubscribeToEvent(E_EXITREQUESTED, ATOMIC_HANDLER(IPCPlayerApp, HandleExitRequest));
            SubscribeToEvent(E_SCREENMODE, ATOMIC_HANDLER(IPCPlayerApp, HandlePlayerWindowChanged));
            SubscribeToEvent(E_WINDOWPOS, ATOMIC_HANDLER(IPCPlayerApp, HandlePlayerWindowChanged));
            SubscribeToEvent(E_UPDATESPAUSEDRESUMED, ATOMIC_HANDLER(IPCPlayerApp, HandleUpdatesPausedResumed));

            if (ipc_->InitWorker((unsigned)id, fd_[0], fd_[1]))
            {
                brokerActive_ = true;
            }
            else if (subprocess_)
            {
                ATOMIC_LOGERROR("IPCPlayerApp::Start() - Unable to initialize IPC Worker");
            }
        }

        if (subprocess_)
        {
            JSVM* vm = JSVM::GetJSVM(0);

            if (!vm->ExecuteMain())
            {
                SendEvent(E_EXITREQUESTED);
            }

            SubscribeToEvent(E_PLAYERQUIT, ATOMIC_HANDLER(IPCPlayerApp, HandleQuit));
        }

        

    }

    void IPCPlayerApp::HandleQuit(StringHash eventType, VariantMap& eventData)
    {
        engine_->Exit();
    }

    void IPCPlayerApp::Stop()
    {
        PlayerApp::Stop();
    }

    void IPCPlayerApp::HandleIPCInitialize(StringHash eventType, VariantMap& eventData)
    {
        brokerActive_ = true;

        SystemUI::DebugHud* debugHud = GetSubsystem<SystemUI::DebugHud>();
        if (debugHud)
            debugHud->SetMode(eventData["debugHudMode"].GetUInt());

    }

    void IPCPlayerApp::HandleJSError(StringHash eventType, VariantMap& eventData)
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

            ATOMIC_LOGERROR("SENDING E_IPCJSERROR");

        }

    }

    void IPCPlayerApp::HandlePlayerWindowChanged(StringHash eventType, VariantMap& eventData)
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        using namespace IPCPlayerWindowChanged;
        VariantMap data;
        data[P_POSX] = graphics->GetWindowPosition().x_;
        data[P_POSY] = graphics->GetWindowPosition().y_;
        data[P_WIDTH] = graphics->GetWidth();
        data[P_HEIGHT] = graphics->GetHeight();
        data[P_MONITOR] = graphics->GetCurrentMonitor();
        data[P_MAXIMIZED] = graphics->GetMaximized();
        ipc_->SendEventToBroker(E_IPCPLAYERWINDOWCHANGED, data);
    }

    void IPCPlayerApp::HandleUpdatesPausedResumed(StringHash eventType, VariantMap& eventData)
    {
        ipc_->SendEventToBroker(E_IPCPLAYERUPDATESPAUSEDRESUMED, eventData);
    }

    void IPCPlayerApp::HandleExitRequest(StringHash eventType, VariantMap& eventData)
    {
        UnsubscribeFromEvent(E_LOGMESSAGE);
        SendEvent(E_PLAYERQUIT);
    }


    void IPCPlayerApp::HandleLogMessage(StringHash eventType, VariantMap& eventData)
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


