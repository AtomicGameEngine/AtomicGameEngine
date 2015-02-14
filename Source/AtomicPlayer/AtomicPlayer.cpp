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

#include <Atomic/Javascript/Javascript.h>

#include "AtomicPlayer.h"

#include <Atomic/DebugNew.h>

#include <Atomic/UI/TBUI.h>

DEFINE_APPLICATION_MAIN(AtomicPlayer);

// fixme
static JSVM* vm = NULL;
static Javascript* javascript = NULL;

AtomicPlayer::AtomicPlayer(Context* context) :
    Application(context)
{
    RegisterEnvironmenttLibrary(context_);
}

void AtomicPlayer::Setup()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    
    engineParameters_["WindowTitle"] = "AtomicPlayer";

#if (ATOMIC_PLATFORM_ANDROID)
    engineParameters_["FullScreen"] = true;
    engineParameters_["ResourcePaths"] = "CoreData;Data;AtomicResources";
#elif ATOMIC_PLATFORM_WEB
    engineParameters_["FullScreen"] = false;
    engineParameters_["ResourcePaths"] = "AtomicResources";
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;

#else
    engineParameters_["ResourcePaths"] = "AtomicResources";
    engineParameters_["FullScreen"] = false;
    engineParameters_["WindowWidth"] = 1280;
    engineParameters_["WindowHeight"] = 720;
#endif

#if ATOMIC_PLATFORM_WINDOWS
    engineParameters_["ResourcePrefixPath"] = "AtomicPlayer_Resources";
#elif ATOMIC_PLATFORM_ANDROID
    //engineParameters_["ResourcePrefixPath"] = "assets";
#elif ATOMIC_PLATFORM_OSX
    engineParameters_["ResourcePrefixPath"] = "../Resources";
#endif

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

    vm->SetModuleSearchPath("Modules");

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
