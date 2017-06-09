//
// Copyright (c) 2008-2014 the Urho3D project.
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

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Core/WorkQueue.h>
#include <Atomic/Engine/EngineDefs.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/IOEvents.h>

#include <Atomic/Resource/ResourceMapRouter.h>
#include <Atomic/IPC/IPCEvents.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>

#include <AtomicGlow/Common/GlowSettings.h>
#include <AtomicGlow/Common/GlowEvents.h>
#include <AtomicGlow/Kernel/BakeModel.h>
#include <AtomicGlow/Kernel/BakeMaterial.h>
#include <AtomicGlow/Kernel/SceneBaker.h>

#include "AtomicGlowApp.h"

using namespace ToolCore;

#ifdef ATOMIC_PLATFORM_OSX
#include <unistd.h>
#endif

#ifdef ATOMIC_PLATFORM_WINDOWS
#include <stdio.h>
#endif


ATOMIC_DEFINE_APPLICATION_MAIN(AtomicGlow::AtomicGlowApp);

namespace AtomicGlow
{

    AtomicGlowApp::AtomicGlowApp(Context* context) :
        IPCClientApp(context)
    {

    }

    AtomicGlowApp::~AtomicGlowApp()
    {

    }

    void AtomicGlowApp::HandleIPCCmd(StringHash eventType, VariantMap& eventData)
    {

        using namespace IPCCmd;

        IPC* ipc = GetSubsystem<IPC>();

        String cmd = eventData[P_COMMAND].GetString().ToLower();
        unsigned id = eventData[P_ID].GetUInt();

        VariantMap result;
        result[P_COMMAND] = cmd;
        result[P_ID] = id;

        if (cmd == "bake")
        {
            timer_.Reset();

            String sceneName = eventData["scenename"].GetString();

            // settings
            VectorBuffer settingsBuffer = eventData["settings"].GetVectorBuffer();
            GlobalGlowSettings.Unpack(settingsBuffer);

            ATOMIC_LOGINFOF("AtomicGlow baking scene: %s", sceneName.CString());

            sceneBaker_->SetStandaloneMode(false);
            sceneBaker_->LoadScene(sceneName);

            using namespace IPCCmdResult;
            result["result"] = "success";

            ipc->SendEventToBroker(E_IPCCMDRESULT, result);

        }

        if (cmd == "quit")
        {
            ATOMIC_LOGINFO("AtomicGlow quit received, exiting");
            exitCode_ = EXIT_SUCCESS;
            engine_->Exit();
        }

    }

    void AtomicGlowApp::HandleLogMessage(StringHash eventType, VariantMap& eventData)
    {
        using namespace LogMessage;

        if (GetBrokerActive())
        {

            if (!GetIPC())
                return;

            VariantMap logEvent;
            logEvent[IPCWorkerLog::P_LEVEL] = eventData[P_LEVEL].GetInt();
            logEvent[IPCWorkerLog::P_MESSAGE] = eventData[P_MESSAGE].GetString();
            GetIPC()->SendEventToBroker(E_IPCWORKERLOG, logEvent);
        }

    }


    void AtomicGlowApp::HandleUpdate(StringHash eventType, VariantMap& eventData)
    {
        // if no scene has been loaded, return
        if (!sceneBaker_->GetScene())
        {
            return;
        }

        if (!GetSubsystem<WorkQueue>()->IsCompleted(M_MAX_UNSIGNED))
        {
            return;
        }

        // if we're done, exit in standalone mode, or send IPC event if
        // running off Glow service
        if (sceneBaker_->GetCurrentLightMode() == GLOW_LIGHTMODE_COMPLETE)
        {
            String resultString = ToString("Scene lit in %i seconds", (int) (timer_.GetMSec(false) / 1000.0f));

            ATOMIC_LOGINFO(resultString);

            UnsubscribeFromEvent(E_UPDATE);

            sceneBaker_->GenerateLightmaps();

            if (sceneBaker_->GetStandaloneMode())
            {
                // TODO: write scene file/lightmaps in standalone mode

                exitCode_ = EXIT_SUCCESS;
                engine_->Exit();
                return;
            }
            else
            {
                using namespace AtomicGlowResult;
                VariantMap eventData;
                eventData[P_SUCCESS] = true;
                eventData[P_RESULT] = resultString;
                eventData[P_BAKEDATA] = sceneBaker_->GetBakeData();
                GetIPC()->SendEventToBroker(E_ATOMICGLOWRESULT, eventData);
            }

            return;
        }

        if (sceneBaker_->GetCurrentLightMode() == GLOW_LIGHTMODE_UNDEFINED)
        {

            // light mode will either move to direct or complete, depending on work to do
            sceneBaker_->Light(GLOW_LIGHTMODE_DIRECT);
            return;
        }

        if (sceneBaker_->GetCurrentLightMode() == GLOW_LIGHTMODE_DIRECT)
        {
            sceneBaker_->LightFinishCycle();

            // light mode will either move to indirect or complete, depending on work to do
            sceneBaker_->Light(GLOW_LIGHTMODE_INDIRECT);
            return;
        }

        if (sceneBaker_->GetCurrentLightMode() == GLOW_LIGHTMODE_INDIRECT)
        {
            sceneBaker_->LightFinishCycle();

            // light mode will either move to indirect or complete, depending on work to do
            sceneBaker_->Light(GLOW_LIGHTMODE_INDIRECT);
            return;
        }

    }

    void AtomicGlowApp::Start()
    {
        // IMPORTANT!!!
        // This needs to be refactored, see // ATOMIC GLOW HACK in AssetDatabase.cpp
        // SharedPtr<ResourceMapRouter> router(new ResourceMapRouter(context_, "__atomic_ResourceCacheMap.json"));
        // IMPORTANT!!!

        if (exitCode_)
            return;

        if (!engine_->Initialize(engineParameters_))
        {
            ErrorExit();
            return;
        }

        context_->RegisterSubsystem(new BakeMaterialCache(context_));
        context_->RegisterSubsystem(new BakeModelCache(context_));

        SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(AtomicGlowApp, HandleUpdate));
        SubscribeToEvent(E_LOGMESSAGE, ATOMIC_HANDLER(AtomicGlowApp, HandleLogMessage));
        SubscribeToEvent(E_IPCMESSAGE, ATOMIC_HANDLER(AtomicGlowApp, HandleUpdate));
        SubscribeToEvent(E_IPCCMD, ATOMIC_HANDLER(AtomicGlowApp, HandleIPCCmd));

        bool ipc = IPCClientApp::Initialize(arguments_);

        ToolSystem* tsystem = GetSubsystem<ToolSystem>();

        // Load project, in read only mode
        if (!tsystem->LoadProject(projectPath_, true))
        {
            ErrorExit(ToString("Unable to load project %s", projectPath_.CString()));
            return;
        }

        sceneBaker_ = new SceneBaker(context_, projectPath_);

        if (!ipc)
        {
            if (!sceneBaker_->LoadScene(scenePath_))
            {
                String message = scenePath_.Length() ? "AtomicGlowApp::Start() - standalone mode unable to load scene: " + scenePath_:
                                                       "AtomicGlowApp::Start() - standalone mode scene not specified";

                ErrorExit(message);
            }
        }

    }

    void AtomicGlowApp::Setup()
    {
        // AtomicGlow is always headless
        engineParameters_["Headless"] = true;

        FileSystem* fileSystem = GetSubsystem<FileSystem>();

        ToolSystem* tsystem = new ToolSystem(context_);
        context_->RegisterSubsystem(tsystem);

        ToolEnvironment* env = new ToolEnvironment(context_);
        context_->RegisterSubsystem(env);

        // Initialize the ToolEnvironment
        if (!env->Initialize(true))
        {
            ErrorExit("Unable to initialize tool environment");
            return;
        }

        for (unsigned i = 0; i < arguments_.Size(); ++i)
        {
            if (arguments_[i].Length() > 1)
            {
                String argument = arguments_[i].ToLower();
                String value = i + 1 < arguments_.Size() ? arguments_[i + 1] : String::EMPTY;

                if (argument == "--project" && value.Length())
                {
                    if (GetExtension(value) == ".atomic")
                    {
                        value = GetPath(value);
                    }

                    if (!fileSystem->DirExists(value))
                    {
                        ErrorExit(ToString("%s project path does not exist", value.CString()));
                        return;
                    }

                    projectPath_ = AddTrailingSlash(value);

                }
                else if (argument == "--scene" && value.Length())
                {
                    scenePath_ = value;

                }

            }
        }

        if (!projectPath_.Length())
        {
            ErrorExit(ToString("%s project path not specified"));
            return;
        }

        engineParameters_.InsertNew("LogName", fileSystem->GetAppPreferencesDir("AtomicEditor", "Logs") + "AtomicGlow.log");

    #ifdef ATOMIC_DEV_BUILD
        engineParameters_["ResourcePrefixPaths"] = env->GetRootSourceDir() + "/Resources/";
        engineParameters_["ResourcePaths"] = ToString("CoreData;EditorData;%sResources;%sCache", projectPath_.CString(), projectPath_.CString());
    #endif

        // TODO: change to using new workerthreadcount command line arg
        // which exposed in editor GlowComponent UI
        // also, check how the number of threads affects light times
        // engineParameters_[EP_WORKER_THREADS_COUNT] = 8;

        IPCClientApp::Setup();

    }


}


