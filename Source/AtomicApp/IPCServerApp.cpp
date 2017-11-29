#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include <AtomicApp/Player/IPCPlayerAppEvents.h>
#include <Atomic/Input/InputEvents.h>

#include "IPCServerApp.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
// PostMessage/PostMessageA windows macro introduced through AppBase.h
#pragma push_macro("UndefPostMessage")
#undef PostMessage
#endif

namespace Atomic
{

    IPCServerApp::IPCServerApp(Context* context) :
        AppBase(context)
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

    bool IPCServerApp::RunIPCClient(const String& projectName, const String& projectPath, const String &addArgs)
    {
        if (clientBroker_.NotNull())
            return false;

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        String projectAssembly = projectName + ".dll";
        String projectExe = projectName + ".exe";

        String clientBinary = "";

        String resourcePath = projectPath + "Resources/" + projectAssembly;

        // TODO: We need to configure project as managed
        if (fileSystem->FileExists(resourcePath))
        {
#ifdef ATOMIC_DEV_BUILD

#ifdef ATOMIC_DEBUG        
            clientBinary = projectPath + "AtomicNET/Debug/Bin/Desktop/" + projectExe;
#else
            clientBinary = projectPath + "AtomicNET/Release/Bin/Desktop/" + projectExe;
#endif

#else
            // TODO: We are using the release build of the managed project here, how and when to use debug?
            clientBinary = projectPath + "AtomicNET/Release/Bin/Desktop/" + projectExe;
#endif


            if (!fileSystem->FileExists(clientBinary))
            {
                ATOMIC_LOGERRORF("Managed client: %s does not exist", clientBinary.CString());
                return false;
            }

        }
        else
        {
            return false;
        }

        Vector<String> vargs;

        
        String args = ToString("--project \"%s\"", projectPath.CString());

        vargs = args.Split(' ');

#ifdef ATOMIC_DEV_BUILD
        vargs.Insert(0, ToString("\"%s/Resources/\"", ATOMIC_ROOT_SOURCE_DIR));
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
        ATOMIC_LOGINFOF("Launching Broker %s %s", clientBinary.CString(), dump.CString());

        IPC* ipc = GetSubsystem<IPC>();
        clientBroker_ = ipc->SpawnWorker(clientBinary, vargs);

        if (clientBroker_)
        {
            SubscribeToEvent(clientBroker_, E_IPCWORKERSTART, ATOMIC_HANDLER(IPCServerApp, HandleIPCWorkerStarted));
            SubscribeToEvent(clientBroker_, E_IPCWORKEREXIT, ATOMIC_HANDLER(IPCServerApp, HandleIPCWorkerExit));
            SubscribeToEvent(clientBroker_, E_IPCWORKERLOG, ATOMIC_HANDLER(IPCServerApp, HandleIPCWorkerLog));
        }

        return clientBroker_.NotNull();
    }

    void IPCServerApp::HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData)
    {
        VariantMap startupData;
        clientBroker_->PostMessage(E_IPCINITIALIZE, startupData);
    }


    void IPCServerApp::HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData)
    {
        if (eventData[IPCWorkerExit::P_BROKER] == clientBroker_)
        {
            clientBroker_ = 0;

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

        VariantMap playerLogData;

        playerLogData["message"] = eventData[P_MESSAGE].GetString();
        playerLogData["level"] = eventData[P_LEVEL].GetInt();

    }
    
    void IPCServerApp::RequestTogglePlayerUpdatesPaused()
    {
        if (!clientBroker_)
        {
            return;
        }

        VariantMap noEventData;
        clientBroker_->PostMessage(E_PAUSERESUMEREQUESTED, noEventData);
    }

    void IPCServerApp::RequestPlayerPauseStep()
    {
        if (!clientBroker_)
        {
            return;
        }

        VariantMap noEventData;
        clientBroker_->PostMessage(E_PAUSESTEPREQUESTED, noEventData);
    }

    void IPCServerApp::RequestPlayerExit()
    {
        if (!clientBroker_)
        {
            return;
        }

        VariantMap noEventData;
        clientBroker_->PostMessage(E_EXITREQUESTED, noEventData);
    }
}

#ifdef ATOMIC_PLATFORM_WINDOWS
#pragma pop_macro("UndefPostMessage")
#endif