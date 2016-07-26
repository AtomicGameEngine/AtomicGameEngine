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

#include <Atomic/Atomic.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/IOEvents.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>
#include <Atomic/UI/UI.h>

// Move me
#include <Atomic/Environment/Environment.h>

#include <AtomicJS/Javascript/Javascript.h>

#include <AtomicPlayer/Player.h>

#include <AtomicNET/NETCore/NETCore.h>
#include "AtomicSharpApp.h"

#include <Atomic/DebugNew.h>

#ifdef __APPLE__
#include <unistd.h>
#endif

DEFINE_APPLICATION_MAIN(AtomicPlayer::AtomicPlayerApp)

namespace AtomicPlayer
{

    extern void jsapi_init_atomicplayer(JSVM* vm);

    AtomicPlayerApp::AtomicPlayerApp(Context* context) :
        Application(context)
    {
    }

    void AtomicPlayerApp::Setup()
    {

#ifdef ATOMIC_3D
        RegisterEnvironmentLibrary(context_);
#endif

        FileSystem* filesystem = GetSubsystem<FileSystem>();

        engineParameters_["WindowTitle"] = "AtomicPlayer";

#if (ATOMIC_PLATFORM_ANDROID)
        engineParameters_["FullScreen"] = true;
        engineParameters_["ResourcePaths"] = "CoreData;PlayerData;Cache;AtomicResources";
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
        engineParameters_["WindowWidth"] = 1280 * .55f;
        engineParameters_["WindowHeight"] = 720 * .55f;
        engineParameters_["ResourcePaths"] = "/Users/josh/Dev/atomic/AtomicGameEngineSharp/Resources/CoreData;/Users/josh/Dev/atomic/AtomicGameEngineSharp/Resources/PlayerData;/Users/josh/Dev/atomic/AtomicExamples/BunnyMark/Resources;/Users/josh/Dev/atomic/AtomicExamples/BunnyMark/";
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
                    SubscribeToEvent(E_LOGMESSAGE, HANDLER(AtomicPlayerApp, HandleLogMessage));
                }
            }
        }

        // Use the script file name as the base name for the log file
        engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicPlayer", "Logs") + "AtomicPlayer.log";
    }

    void AtomicPlayerApp::Start()
    {
        Application::Start();

        // Instantiate and register NETCore subsystem
        context_->RegisterSubsystem(new NETCore(context_));

        // Instantiate and register the Javascript subsystem
        Javascript* javascript = new Javascript(context_);
        context_->RegisterSubsystem(javascript);

        vm_ = javascript->InstantiateVM("MainVM");
        vm_->InitJSContext();

        UI* ui = GetSubsystem<UI>();
        ui->Initialize("DefaultUI/language/lng_en.tb.txt");
        ui->LoadDefaultPlayerSkin();

        vm_->SetModuleSearchPaths("Modules");

        // Instantiate and register the Player subsystem
        context_->RegisterSubsystem(new AtomicPlayer::Player(context_));
        AtomicPlayer::jsapi_init_atomicplayer(vm_);

        JSVM* vm = JSVM::GetJSVM(0);

        if (!vm->ExecuteMain())
        {
            SendEvent(E_EXITREQUESTED);
        }


        return;
    }

    void AtomicPlayerApp::Stop()
    {

        vm_ = 0;
        context_->RemoveSubsystem<Javascript>();
        // make sure JSVM is really down and no outstanding refs
        // as if not, will hold on engine subsystems, which is bad
        assert(!JSVM::GetJSVM(0));

        context_->RemoveSubsystem<NETCore>();

        Application::Stop();

    }

    void AtomicPlayerApp::HandleScriptReloadStarted(StringHash eventType, VariantMap& eventData)
    {
    }

    void AtomicPlayerApp::HandleScriptReloadFinished(StringHash eventType, VariantMap& eventData)
    {
    }

    void AtomicPlayerApp::HandleScriptReloadFailed(StringHash eventType, VariantMap& eventData)
    {
        ErrorExit();
    }

    void AtomicPlayerApp::HandleLogMessage(StringHash eventType, VariantMap& eventData)
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

    int AtomicPlayerApp::Initialize()
    {
        try
        {
            Setup();
            if (exitCode_)
                return exitCode_;

            if (!engine_->Initialize(engineParameters_))
            {
                ErrorExit();
                return exitCode_;
            }

            Start();

            return exitCode_;

        }
        catch (std::bad_alloc&)
        {
            ErrorDialog(GetTypeName(), "An out-of-memory error occurred. The application will now exit.");
            return EXIT_FAILURE;
        }
    }

    bool AtomicPlayerApp::RunFrame()
    {
        try
        {
            if (!engine_->IsExiting())
            {
                engine_->RunFrame();
                return true;
            }
            else
                return false;
        }
        catch (std::bad_alloc&)
        {
            ErrorDialog(GetTypeName(), "An out-of-memory error occurred. The application will now exit.");
            return EXIT_FAILURE;
        }
    }

    int AtomicPlayerApp::Shutdown()
    {
        Stop();

        return exitCode_;
    }

}

static SharedPtr<Atomic::Context> sContext;
static SharedPtr<AtomicPlayer::AtomicPlayerApp> sApplication;

extern "C" {

#ifdef ATOMIC_PLATFORM_WINDOWS
#pragma warning(disable: 4244) // possible loss of data
#define ATOMIC_EXPORT_API __declspec(dllexport)
#else
#define ATOMIC_EXPORT_API
#endif

ATOMIC_EXPORT_API int atomicsharp_initialize()
{
    sContext = new Atomic::Context();
    sApplication = new AtomicPlayer::AtomicPlayerApp(sContext);
    return sApplication->Initialize();
}

ATOMIC_EXPORT_API bool atomicsharp_runframe()
{
    if (!sApplication->RunFrame())
    {
        sApplication->Shutdown();

        sApplication = 0;
        sContext = 0;
        return false;
    }

    return true;
}

}
