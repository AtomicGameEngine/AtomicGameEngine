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
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>
#include <Atomic/UI/UI.h>

// Move me
#include <Atomic/Environment/Environment.h>

#include <AtomicJS/Javascript/Javascript.h>


#include "../PlayerMode/AEPlayerMode.h"
#include <AtomicPlayer/Player.h>

#include "AEPlayerApp.h"

#include <Atomic/DebugNew.h>

#ifdef __APPLE__
#include <unistd.h>
#endif

namespace AtomicPlayer
{
    extern void jsapi_init_atomicplayer(JSVM* vm);
}

namespace AtomicEditor
{

AEPlayerApplication::AEPlayerApplication(Context* context) :
    AEEditorCommon(context)
{
}

void AEPlayerApplication::Setup()
{
    AEEditorCommon::Setup();

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

    engineParameters_["LogLevel"] = LOG_DEBUG;

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
            else if (argument == "--project" && value.Length())
            {
                engineParameters_["ResourcePrefixPath"] = "";

                // This works for a local dev build, --editor-resource-paths command below is for
                // launching from AtomicEditor (IPC)

                value = AddTrailingSlash(value);

#ifdef ATOMIC_DEV_BUILD

                String resourcePaths = ToString("%s/Resources/CoreData;%s/Resources/PlayerData;%s/;%s/Resources;%s;%sCache",
                         ATOMIC_ROOT_SOURCE_DIR, ATOMIC_ROOT_SOURCE_DIR, value.CString(), value.CString(), value.CString(), value.CString());

#else

#ifdef __APPLE__
                engineParameters_["ResourcePrefixPath"] = "../Resources";
#else
				engineParameters_["ResourcePrefixPath"] = filesystem->GetProgramDir() + "Resources";
#endif

                String resourcePaths = ToString("CoreData;PlayerData;%s/;%s/Resources;%s;%sCache",
                                                              value.CString(), value.CString(), value.CString(), value.CString());
#endif

                LOGINFOF("Adding ResourcePaths: %s", resourcePaths.CString());

                engineParameters_["ResourcePaths"] = resourcePaths;

            }
        }
    }



    // Use the script file name as the base name for the log file
    engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicPlayer", "Logs") + "AtomicPlayer.log";
}

void AEPlayerApplication::Start()
{
    AEEditorCommon::Start();

    UI* ui = GetSubsystem<UI>();
    ui->Initialize("DefaultUI/language/lng_en.tb.txt");
    ui->LoadDefaultPlayerSkin();

    context_->RegisterSubsystem(new PlayerMode(context_));
    PlayerMode* playerMode = GetSubsystem<PlayerMode>();
    playerMode->ProcessArguments();

    SubscribeToEvent(E_JSERROR, HANDLER(AEPlayerApplication, HandleJSError));

    vm_->SetModuleSearchPaths("Modules");

    // Instantiate and register the Player subsystem
    context_->RegisterSubsystem(new AtomicPlayer::Player(context_));
    AtomicPlayer::jsapi_init_atomicplayer(vm_);

    if (!playerMode->launchedByEditor())
    {
        JSVM* vm = JSVM::GetJSVM(0);

        if (!vm->ExecuteMain())
        {
            SendEvent(E_EXITREQUESTED);
        }

    }
    return;
}

void AEPlayerApplication::Stop()
{
    AEEditorCommon::Stop();
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
    //String errStack = eventData[P_ERRORSTACK].GetString();
    String errMessage = eventData[P_ERRORMESSAGE].GetString();
    String errFilename = eventData[P_ERRORFILENAME].GetString();
    int errLineNumber = eventData[P_ERRORLINENUMBER].GetInt();

    String errorString = ToString("%s - %s - Line: %i",
                                  errFilename.CString(), errMessage.CString(), errLineNumber);

}


}
