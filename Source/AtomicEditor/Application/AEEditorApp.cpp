// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Core/StringUtils.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Atomic3D/AnimatedModel.h>

#include <Atomic/UI/UI.h>

#include <AtomicJS/Javascript/Javascript.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/License/LicenseEvents.h>
#include <ToolCore/License/LicenseSystem.h>

#include "../EditorMode/AEEditorMode.h"

#include "AEEditorApp.h"

using namespace ToolCore;

namespace ToolCore
{
    extern void jsapi_init_toolcore(JSVM* vm);
}

namespace AtomicEditor
{

extern void jsapi_init_editor(JSVM* vm);

AEEditorApp::AEEditorApp(Context* context) :
    AEEditorCommon(context)
{

}

void AEEditorApp::Start()
{
    AEEditorCommon::Start();

    context_->RegisterSubsystem(new EditorMode(context_));

    vm_->SetModuleSearchPaths("AtomicEditor/out");

    // Do not create bone structure by default when in the editor
    // this can be toggled temporarily, for example to setup an animation preview
    AnimatedModel::SetBoneCreationEnabled(false);

    // move UI initialization to JS
    UI* ui = GetSubsystem<UI>();
    ui->Initialize("AtomicEditor/resources/language/lng_en.tb.txt");

    SubscribeToEvent(E_JSERROR, HANDLER(AEEditorApp, HandleJSError));
    SubscribeToEvent(E_EXITREQUESTED, HANDLER(AEEditorApp, HandleExitRequested));

    jsapi_init_toolcore(vm_);
    jsapi_init_editor(vm_);

    duk_get_global_string(vm_->GetJSContext(), "require");
    duk_push_string(vm_->GetJSContext(), "main");
    if (duk_pcall(vm_->GetJSContext(), 1) != 0)
    {
        vm_->SendJSErrorEvent();
        ErrorExit("Error executing main.js");
    }


    GetSubsystem<LicenseSystem>()->Initialize();

}

void AEEditorApp::Setup()
{
    context_->SetEditorContext(true);

    ToolEnvironment* env = new ToolEnvironment(context_);
    context_->RegisterSubsystem(env);

    ToolSystem* system = new ToolSystem(context_);
    context_->RegisterSubsystem(system);

#ifdef ATOMIC_DEV_BUILD

    if (!env->InitFromJSON())
    {
        ErrorExit(ToString("Unable to initialize tool environment from %s", env->GetDevConfigFilename().CString()));
        return;
    }
#else

    env->InitFromPackage();

#endif

    engineParameters_["WindowTitle"] = "AtomicEditor";
    engineParameters_["WindowResizable"] = true;
    engineParameters_["FullScreen"] = false;
    engineParameters_["LogLevel"] = LOG_DEBUG;

    FileSystem* filesystem = GetSubsystem<FileSystem>();
    engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicEditor", "Logs") + "AtomicEditor.log";

#ifdef ATOMIC_PLATFORM_OSX
    engineParameters_["WindowIcon"] = "Images/AtomicLogo32.png";
#endif

#ifdef ATOMIC_DEV_BUILD
    engineParameters_["ResourcePrefixPath"] = "";
    String ScriptPath = env->GetRootSourceDir() + "Script";
    String resourcePaths = env->GetCoreDataDir() + ";" +  env->GetEditorDataDir() + ";" + ScriptPath;
    engineParameters_["ResourcePaths"] = resourcePaths;
#else

#ifdef ATOMIC_PLATFORM_OSX
    engineParameters_["ResourcePrefixPath"] = "../Resources";
    
#else
	engineParameters_["ResourcePrefixPath"] = filesystem->GetProgramDir() + "Resources";
#endif

	engineParameters_["ResourcePaths"] = "CoreData;EditorData;Script";

#endif // ATOMIC_DEV_BUILD


}

void AEEditorApp::Stop()
{
    AEEditorCommon::Stop();
}

void AEEditorApp::HandleExitRequested(StringHash eventType, VariantMap& eventData)
{
}

void AEEditorApp::HandleJSError(StringHash eventType, VariantMap& eventData)
{
    using namespace JSError;
    //String errName = eventData[P_ERRORNAME].GetString();
    String errMessage = eventData[P_ERRORMESSAGE].GetString();
    String errFilename = eventData[P_ERRORFILENAME].GetString();
    //String errStack = eventData[P_ERRORSTACK].GetString();
    int errLineNumber = vm_->GetRealLineNumber("AtomicEditor/out/" + errFilename, eventData[P_ERRORLINENUMBER].GetInt());
    
    String errorString = ToString("%s - %s - Line: %i", errFilename.CString(), errMessage.CString(), errLineNumber);


    ErrorExit(errorString);

}




}
