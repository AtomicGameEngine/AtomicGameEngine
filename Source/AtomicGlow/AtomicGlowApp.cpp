//
// Copyright (c) 2008-2016 the Urho3D project.
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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
#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>

#include "AtomicGlowApp.h"

// Test

#include "ModelAOBake.h"

#ifdef ATOMIC_PLATFORM_OSX
#include <unistd.h>
#endif

#ifdef ATOMIC_PLATFORM_WINDOWS
#include <stdio.h>
#endif

using namespace ToolCore;

ATOMIC_DEFINE_APPLICATION_MAIN(AtomicGlow::AtomicGlowApp)

namespace AtomicGlow
{

    AtomicGlowApp::AtomicGlowApp(Context* context) :
        IPCClientApp(context)
    {
        SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(AtomicGlowApp, HandleUpdate));
    }

    void AtomicGlowApp::Setup()
    {
        IPCClientApp::Setup();

        // AtomicGlow is always headless
        engineParameters_["Headless"] = true;

        FileSystem* filesystem = GetSubsystem<FileSystem>();
        engineParameters_.InsertNew("LogName", filesystem->GetAppPreferencesDir("AtomicEditor", "Logs") + "AtomicGlow.log");

        ToolSystem* tsystem = new ToolSystem(context_);
        context_->RegisterSubsystem(tsystem);

        ToolEnvironment* env = new ToolEnvironment(context_);
        context_->RegisterSubsystem(env);

        String projectPath;

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

                    if (filesystem->DirExists(value))
                    {

                    }
                    else
                    {
                        ErrorExit(ToString("%s project path does not exist", value.CString()));
                    }

                    projectPath = AddTrailingSlash(value);

                }
            }
        }

        if (!env->InitFromJSON())
        {
            ErrorExit(ToString("Unable to initialize tool environment from %s", env->GetDevConfigFilename().CString()));
            return;
        }

        tsystem->SetCLI();
        tsystem->SetDataPath(env->GetRootSourceDir() + "/Resources/");

        engineParameters_["ResourcePrefixPaths"] = env->GetRootSourceDir() + "/Resources/";
        engineParameters_["ResourcePaths"] = ToString("CoreData;EditorData;%sResources;%sCache", projectPath.CString(), projectPath.CString());


    }

    int AtomicGlowApp::Initialize()
    {
        Setup();
        if (exitCode_)
            return exitCode_;

        if (!engine_->Initialize(engineParameters_))
        {
            ErrorExit("engine_->Initialize(engineParameters_) failed");
            return exitCode_;
        }

        Start();
        if (exitCode_)
            return exitCode_;

        IPCClientApp::Initialize(arguments_);

        return 0;
    }

    void AtomicGlowApp::HandleUpdate(StringHash eventType, VariantMap& eventData)
    {

        SharedPtr<ModelAOBake> baker(new ModelAOBake(context_));
        baker->LoadModel("5630bdba26c7d8ab29daf2ceaba45cd2.mdl");

        exitCode_ = EXIT_SUCCESS;
        engine_->Exit();
    }

    void AtomicGlowApp::Stop()
    {
        Application::Stop();
    }

    void AtomicGlowApp::ErrorExit(const String& message)
    {
        engine_->Exit();
        exitCode_ = EXIT_FAILURE;

        // Only for WIN32, otherwise the error messages would be double posted on Mac OS X and Linux platforms
        if (!message.Length())
        {
#ifdef WIN32
            Atomic::ErrorExit(startupErrors_.Length() ? startupErrors_ :
                                                        "Application has been terminated due to unexpected error.", exitCode_);
#endif
        }
        else
            Atomic::ErrorExit(message, exitCode_);
    }


}
