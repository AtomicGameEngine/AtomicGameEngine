//
// Copyright (c) 2008-2014 the Urho3D project.
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

#include <Atomic/Atomic.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/IOEvents.h>
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCWorker.h>

// Move me
#include <Atomic/Environment/Environment.h>

#include <AtomicJS/Javascript/Javascript.h>

#include "AEPlayerApplication.h"

#include <Atomic/DebugNew.h>

#include <Atomic/UI/UI.h>

#ifdef __APPLE__
#include <unistd.h>
#endif

namespace AtomicEditor
{

// fixme
static JSVM* vm = NULL;
static Javascript* javascript = NULL;

AEPlayerApplication::AEPlayerApplication(Context* context) :
    Application(context)
{
    fd_[0] = INVALID_IPCHANDLE_VALUE;
    fd_[1] = INVALID_IPCHANDLE_VALUE;

#ifdef ATOMIC_3D
    RegisterEnvironmentLibrary(context_);
#endif

}

void AEPlayerApplication::Setup()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();

    engineParameters_["WindowTitle"] = "AtomicPlayer";

#if (ATOMIC_PLATFORM_ANDROID)
    engineParameters_["FullScreen"] = true;
    engineParameters_["ResourcePaths"] = "CoreData;AtomicResources";
#elif ATOMIC_PLATFORM_WEB
    engineParameters_["FullScreen"] = false;
    engineParameters_["ResourcePaths"] = "AtomicResources";
    // engineParameters_["WindowWidth"] = 1280;
    // engineParameters_["WindowHeight"] = 720;
#elif ATOMIC_PLATFORM_IOS
    engineParameters_["FullScreen"] = false;
    engineParameters_["ResourcePaths"] = "AtomicResources";
#else
    engineParameters_["FullScreen"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
#endif

#if ATOMIC_PLATFORM_WINDOWS
    engineParameters_["WindowIcon"] = "Images/AtomicLogo32.png";
    engineParameters_["ResourcePrefixPath"] = "AtomicPlayer_Resources";
#elif ATOMIC_PLATFORM_ANDROID
    //engineParameters_["ResourcePrefixPath"] = "assets";
#elif ATOMIC_PLATFORM_OSX
    engineParameters_["ResourcePrefixPath"] = "../Resources";
#endif

    const Vector<String>& arguments = GetArguments();

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1)
        {
            String argument = arguments[i].ToLower();
            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            if (argument == "--log-std")
            {
                SubscribeToEvent(E_LOGMESSAGE, HANDLER(AEPlayerApplication, HandleLogMessage));
            }

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
            else if (argument == "--project" && value.Length())
            {
                engineParameters_["ResourcePrefixPath"] = "";

                // This works for a local dev build, --editor-resource-paths command below is for
                // launching from AtomicEditor (IPC)

                String resourcePaths = ToString("%s/Data/AtomicPlayer/Resources/CoreData;%s/Data/AtomicPlayer/Resources/PlayerData;%s/Resources",
                         ATOMIC_ROOT_SOURCE_DIR, ATOMIC_ROOT_SOURCE_DIR, value.CString());

                LOGINFOF("Adding ResourcePaths: %s", resourcePaths.CString());

                engineParameters_["ResourcePaths"] = resourcePaths;

            }
            else if (argument == "--editor-resource-paths" && value.Length())
            {
                // "/Users/josh/Dev/atomic/AtomicGameEngine/Data/AtomicPlayer/Resources/CoreData!/Users/josh/Dev/atomic/AtomicGameEngine/Data/AtomicPlayer/Resources/PlayerData!/Users/josh/Dev/atomic/AtomicExamples/NewSpaceGame/Resources"
                engineParameters_["ResourcePrefixPath"] = "";
                value.Replace("!", ";");
                engineParameters_["ResourcePaths"] = value;
            }
        }
    }

    // Use the script file name as the base name for the log file
    engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicPlayer", "Logs") + "AtomicPlayer.log";
}

void AEPlayerApplication::HandleHelloFromBroker(StringHash eventType, VariantMap& eventData)
{
    //assert(eventData[HelloFromBroker::P_HELLO].GetString() == "Hello");
    //assert(eventData[HelloFromBroker::P_LIFETHEUNIVERSEANDEVERYTHING].GetInt() == 42);

    LOGERROR("Passed Test!");
}

void AEPlayerApplication::Start()
{

    //SubscribeToEvent(E_IPCHELLOFROMBROKER, HANDLER(AEPlayerApplication, HandleHelloFromBroker));
    SubscribeToEvent(E_JSERROR, HANDLER(AEPlayerApplication, HandleJSError));


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
        IPC* ipc = new IPC(context_);
        context_->RegisterSubsystem(ipc);
        //ipc->InitWorker(fd_[0], fd_[1]);
    }
#endif

    // Instantiate and register the Javascript subsystem
    javascript = new Javascript(context_);
    context_->RegisterSubsystem(javascript);

    vm = javascript->InstantiateVM("MainVM");
    vm->InitJSContext();

    vm->SetModuleSearchPaths("Modules");

    if (!vm->ExecuteMain())
    {
        ErrorExit("Error executing Scripts/main.js");
    }

    return;
}

void AEPlayerApplication::Stop()
{
    context_->RemoveSubsystem<Javascript>();
}

void AEPlayerApplication::HandleScriptReloadStarted(StringHash eventType, VariantMap& eventData)
{
}

void AEPlayerApplication::HandleScriptReloadFinished(StringHash eventType, VariantMap& eventData)
{
}

void AEPlayerApplication::HandleScriptReloadFailed(StringHash eventType, VariantMap& eventData)
{
    ErrorExit();
}

void AEPlayerApplication::HandleLogMessage(StringHash eventType, VariantMap& eventData)
{
    using namespace LogMessage;

    int level = eventData[P_LEVEL].GetInt();
    // The message may be multi-line, so split to rows in that case
    Vector<String> rows = eventData[P_MESSAGE].GetString().Split('\n');

    for (unsigned i = 0; i < rows.Size(); ++i)
    {
        if (level == LOG_ERROR)
        {
            fprintf(stderr, "%s\n", rows[i].CString());
        }
        else
        {
            fprintf(stdout, "%s\n", rows[i].CString());
        }
    }
        
}

void AEPlayerApplication::HandleJSError(StringHash eventType, VariantMap& eventData)
{
    using namespace JSError;
    //String errName = eventData[P_ERRORNAME].GetString();
    String errMessage = eventData[P_ERRORMESSAGE].GetString();
    String errFilename = eventData[P_ERRORFILENAME].GetString();
    //String errStack = eventData[P_ERRORSTACK].GetString();
    int errLineNumber = eventData[P_ERRORLINENUMBER].GetInt();

    String errorString = ToString("%s - %s - Line: %i",
                                  errFilename.CString(), errMessage.CString(), errLineNumber);

    ErrorExit(errorString);

}


}
