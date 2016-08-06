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
#include <Atomic/Engine/EngineConfig.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/IOEvents.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>
#include <Atomic/Script/ScriptSystem.h>
#include <Atomic/UI/UI.h>

// Move me
#include <Atomic/Environment/Environment.h>

#include <AtomicJS/Javascript/Javascript.h>

#include <AtomicPlayer/Player.h>

#include "AtomicPlayer.h"

#include <Atomic/DebugNew.h>

#ifdef __APPLE__
#include <unistd.h>
#endif

ATOMIC_DEFINE_APPLICATION_MAIN(AtomicPlayer::AtomicPlayerApp)

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
    context_->RegisterSubsystem(new ScriptSystem(context_));

    // Read the engine configuration
    ReadEngineConfig();

    engineParameters_.InsertNew("WindowTitle", "AtomicPlayer");

#if (ATOMIC_PLATFORM_ANDROID)
    engineParameters_.InsertNew("FullScreen", true);
    engineParameters_.InsertNew("ResourcePaths", "CoreData;PlayerData;Cache;AtomicResources");
#elif ATOMIC_PLATFORM_WEB
    engineParameters_.InsertNew("FullScreen", false);
    engineParameters_.InsertNew("ResourcePaths", "AtomicResources");
    // engineParameters_.InsertNew("WindowWidth", 1280);
    // engineParameters_.InsertNew("WindowHeight", 720);
#elif ATOMIC_PLATFORM_IOS
    engineParameters_.InsertNew("FullScreen", false);
    engineParameters_.InsertNew("ResourcePaths", "AtomicResources");
#else
    engineParameters_.InsertNew("FullScreen", false);
    engineParameters_.InsertNew("WindowWidth", 1280);
    engineParameters_.InsertNew("WindowHeight", 720);
    engineParameters_.InsertNew("ResourcePaths", "AtomicResources");
#endif

#if ATOMIC_PLATFORM_WINDOWS || ATOMIC_PLATFORM_LINUX

    engineParameters_.InsertNew("WindowIcon", "Images/AtomicLogo32.png");
    engineParameters_.InsertNew("ResourcePrefixPaths", "AtomicPlayer_Resources");

#elif ATOMIC_PLATFORM_ANDROID
    //engineParameters_.InsertNew("ResourcePrefixPaths"], "assets");
#elif ATOMIC_PLATFORM_OSX
    engineParameters_.InsertNew("ResourcePrefixPaths", "../Resources");
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
                SubscribeToEvent(E_LOGMESSAGE, ATOMIC_HANDLER(AtomicPlayerApp, HandleLogMessage));
            }
        }
    }

    // Use the script file name as the base name for the log file
    engineParameters_.InsertNew("LogName", filesystem->GetAppPreferencesDir("AtomicPlayer", "Logs") + "AtomicPlayer.log");
}

void AtomicPlayerApp::Start()
{
    Application::Start();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    // Instantiate and register the Javascript subsystem
    Javascript* javascript = new Javascript(context_);
    context_->RegisterSubsystem(javascript);

    vm_ = javascript->InstantiateVM("MainVM");
    vm_->InitJSContext();

    UI* ui = GetSubsystem<UI>();
    ui->Initialize("DefaultUI/language/lng_en.tb.txt");
    ui->LoadDefaultPlayerSkin();

    SubscribeToEvent(E_JSERROR, ATOMIC_HANDLER(AtomicPlayerApp, HandleJSError));

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

void AtomicPlayerApp::HandleJSError(StringHash eventType, VariantMap& eventData)
{
    using namespace JSError;
    //String errName = eventData[P_ERRORNAME].GetString();
    //String errStack = eventData[P_ERRORSTACK].GetString();
    String errMessage = eventData[P_ERRORMESSAGE].GetString();
    String errFilename = eventData[P_ERRORFILENAME].GetString();
    int errLineNumber = eventData[P_ERRORLINENUMBER].GetInt();

    String errorString = ToString("%s - %s - Line: %i",
                                  errFilename.CString(), errMessage.CString(), errLineNumber);

}

void AtomicPlayerApp::ReadEngineConfig()
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    String filename = fileSystem->GetProgramDir() + "Settings/Engine.json";

    if (!fileSystem->FileExists(filename))
        return;

    if (EngineConfig::LoadFromFile(context_, filename))
    {
        EngineConfig::ApplyConfig(engineParameters_);
    }
}

}
