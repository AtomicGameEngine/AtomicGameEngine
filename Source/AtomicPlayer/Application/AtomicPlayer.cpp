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
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>

// Move me
#include <Atomic/Environment/Environment.h>

#include <AtomicJS/Javascript/Javascript.h>

#include "AtomicPlayer.h"

#include <Atomic/DebugNew.h>

#include <Atomic/UI/UI.h>

DEFINE_APPLICATION_MAIN(AtomicPlayer);

// fixme
static JSVM* vm = NULL;
static Javascript* javascript = NULL;

AtomicPlayer::AtomicPlayer(Context* context) :
    Application(context)
{
#ifdef ATOMIC_3D
    RegisterEnvironmentLibrary(context_);
#endif
}

void AtomicPlayer::Setup()
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
    engineParameters_["ResourcePaths"] = "AtomicResources";
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

            if (argument == "--editor-resource-paths" && value.Length())
            {
                engineParameters_["ResourcePrefixPath"] = "";
                value.Replace("!", ";");
                engineParameters_["ResourcePaths"] = value;
            }
        }
    }

    // Use the script file name as the base name for the log file
    engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicPlayer", "Logs") + "AtomicPlayer.log";
}

void AtomicPlayer::Start()
{
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

void AtomicPlayer::Stop()
{
    context_->RemoveSubsystem<Javascript>();
}

void AtomicPlayer::HandleScriptReloadStarted(StringHash eventType, VariantMap& eventData)
{
}

void AtomicPlayer::HandleScriptReloadFinished(StringHash eventType, VariantMap& eventData)
{
}

void AtomicPlayer::HandleScriptReloadFailed(StringHash eventType, VariantMap& eventData)
{
    ErrorExit();
}
