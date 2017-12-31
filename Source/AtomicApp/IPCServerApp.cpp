#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/IO/IOEvents.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/CoreEvents.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCWorker.h>
#include <Atomic/IPC/IPCBroker.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/Project/Project.h>
#include <ToolCore/Project/ProjectSettings.h>

#include <Atomic/UI/SystemUI/DebugHud.h>
#include <AtomicApp/Player/IPCPlayerAppEvents.h>

#include <AtomicJS/Javascript/JSIPCEvents.h>

#include <Atomic/Input/InputEvents.h>

#include "IPCServerApp.h"

#pragma push_macro("PostMessage")
#undef PostMessage

using namespace ToolCore;

namespace Atomic
{

    IPCServerApp::IPCServerApp(Context* context) :
        AppBase(context),
        playerEnabled_(false)
    {
    }

    IPCServerApp::~IPCServerApp()
    {
    }

    void IPCServerApp::Setup()
    {
        AppBase::Setup();

        // Register IPC system
        context_->RegisterSubsystem(new IPC(context_));

        ToolEnvironment* env = new ToolEnvironment(context_);
        context_->RegisterSubsystem(env);

        env->Initialize();

        ToolSystem* system = new ToolSystem(context_);
        context_->RegisterSubsystem(system);

        engineParameters_["Headless"] = true;
        engineParameters_["LogLevel"] = LOG_INFO;
    }

    void IPCServerApp::Stop()
    {
        IPC* ipc = GetSubsystem<IPC>();

        if (ipc)
        {
            ipc->Shutdown();
        }

        AppBase::Stop();
    }

    bool IPCServerApp::RunIPCPlayer(const String& projectName, const String& projectPath, const String &addArgs)
    {
        /* Params to pass in (possibly) from PE
        --windowposx 244
        --windowposy 157
        --windowwidth 1440
        --windowheight 806
        --resizable

        ALSO STUFF TO TELL VSEATOMIC WHICH CUTSCENE TO LAUNCH..app delegate will need to parse run params I think

        */

        if (playerBroker_.NotNull())
            return false;

        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        ToolSystem* tsystem = GetSubsystem<ToolSystem>();
        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

        String projectAssembly = projectName + ".dll";
        String projectExe = projectName + ".exe";

        String playerBinary = "";

        String resourcePath = projectPath + "Resources/" + projectAssembly;

        // TODO: We need to configure project as managed
        if (fileSystem->FileExists(resourcePath))
        {
#ifdef ATOMIC_DEV_BUILD

#ifdef ATOMIC_DEBUG        
            playerBinary = projectPath + "AtomicNET/Debug/Bin/Desktop/" + projectExe;
#else
            playerBinary = projectPath + "AtomicNET/Release/Bin/Desktop/" + projectExe;
#endif

#else
            // TODO: We are using the release build of the managed project here, how and when to use debug?
            playerBinary = projectPath + "AtomicNET/Release/Bin/Desktop/" + projectExe;
#endif


            if (!fileSystem->FileExists(playerBinary))
            {
                ATOMIC_LOGERRORF("Managed player: %s does not exist", playerBinary.CString());
                return false;
            }

        }
        else
        {
            return false;
        }

        Vector<String> vargs;

        String args = ToString("--player --project \"%s\"", projectPath.CString());

        vargs = args.Split(' ');

#ifdef ATOMIC_DEV_BUILD
        vargs.Insert(0, ToString("\"%s/Resources/\"", tenv->GetRootSourceDir().CString()));
#else

#ifdef ATOMIC_PLATFORM_OSX
        vargs.Insert(0, ToString("\"%s\"", (fileSystem->GetProgramDir() + "../Resources/").CString()));
#else
        vargs.Insert(0, ToString("\"%s\"", (fileSystem->GetProgramDir() + "Resources/").CString()));
#endif

#endif
        vargs.Insert(0, "--resourcePrefix");

        if (addArgs.Length() > 0)
            vargs.Insert(0, addArgs.Split(' '));


        String dump;
        dump.Join(vargs, " ");
        ATOMIC_LOGINFOF("Launching Broker %s %s", playerBinary.CString(), dump.CString());

        IPC* ipc = GetSubsystem<IPC>();
        playerBroker_ = ipc->SpawnWorker(playerBinary, vargs);

        if (playerBroker_)
        {
            SubscribeToEvent(playerBroker_, E_IPCWORKERSTART, ATOMIC_HANDLER(IPCServerApp, HandleIPCWorkerStarted));
            SubscribeToEvent(playerBroker_, E_IPCJSERROR, ATOMIC_HANDLER(IPCServerApp, HandleIPCJSError));
            SubscribeToEvent(playerBroker_, E_IPCWORKEREXIT, ATOMIC_HANDLER(IPCServerApp, HandleIPCWorkerExit));
            SubscribeToEvent(playerBroker_, E_IPCWORKERLOG, ATOMIC_HANDLER(IPCServerApp, HandleIPCWorkerLog));
        }

        return playerBroker_.NotNull();
    }

    void IPCServerApp::HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData)
    {
        VariantMap startupData;
        DebugHud* debugHud = GetSubsystem<DebugHud>();

        startupData["debugHudMode"] = debugHud ? debugHud->GetMode() : (unsigned)0;
        startupData["debugHudProfilerMode"] = (unsigned)(debugHud ? debugHud->GetProfilerMode() : DEBUG_HUD_PROFILE_PERFORMANCE);

        playerBroker_->PostMessage(E_IPCINITIALIZE, startupData);

        playerEnabled_ = true;
    }


    void IPCServerApp::HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData)
    {
        if (eventData[IPCWorkerExit::P_BROKER] == playerBroker_)
        {
            playerBroker_ = 0;
            playerEnabled_ = false;

            UnsubscribeFromEvent(E_IPCWORKERSTART);
            UnsubscribeFromEvent(E_IPCPLAYERPAUSERESUMEREQUEST);
            UnsubscribeFromEvent(E_IPCPLAYERUPDATESPAUSEDRESUMED);
            UnsubscribeFromEvent(E_IPCPLAYERPAUSESTEPREQUEST);
            UnsubscribeFromEvent(E_IPCPLAYEREXITREQUEST);
        }
        else
        {
            ATOMIC_LOGERROR("IPCServerApp::HandleIPCWorkerExit - Unknown Broker");
        }
    }

    void IPCServerApp::HandleIPCWorkerLog(StringHash eventType, VariantMap& eventData)
    {
        using namespace IPCWorkerLog;

        // convert to a player log

        VariantMap playerLogData;

        playerLogData["message"] = eventData[P_MESSAGE].GetString();
        playerLogData["level"] = eventData[P_LEVEL].GetInt();

        //SendEvent("EditorPlayerLog", playerLogData);

    }

    void IPCServerApp::HandleIPCJSError(StringHash eventType, VariantMap& eventData)
    {

    }
    
    void IPCServerApp::RequestTogglePlayerUpdatesPaused()
    {
        if (!playerBroker_)
        {
            return;
        }

        VariantMap noEventData;
        playerBroker_->PostMessage(E_PAUSERESUMEREQUESTED, noEventData);
    }

    void IPCServerApp::RequestPlayerPauseStep()
    {
        if (!playerBroker_)
        {
            return;
        }

        VariantMap noEventData;
        playerBroker_->PostMessage(E_PAUSESTEPREQUESTED, noEventData);
    }

    void IPCServerApp::RequestPlayerExit()
    {
        if (!playerBroker_)
        {
            return;
        }

        VariantMap noEventData;
        playerBroker_->PostMessage(E_EXITREQUESTED, noEventData);
    }

}

#pragma pop_macro("PostMessage")