//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Engine/Engine.h>
#include <Atomic/Input/Input.h>

#include <Atomic/IPC/IPC.h>

// Move me to Engine
#include <Atomic/Environment/Environment.h>

#include <Atomic/Script/ScriptSystem.h>
#include <AtomicJS/Javascript/Javascript.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>

#include <Atomic/IO/File.h>

#ifdef ATOMIC_DOTNET
#include <AtomicNET/NETCore/NETHost.h>
#include <AtomicNET/NETCore/NETCore.h>
#include <AtomicNET/NETScript/NETScript.h>
#endif

#include "../Components/EditorComponents.h"

#include "AEEditorCommon.h"

namespace Atomic
{
    void jsapi_init_atomicnet(JSVM* vm);
}

using namespace ToolCore;

namespace ToolCore
{
    extern void jsapi_init_toolcore(JSVM* vm);
}


namespace AtomicEditor
{

AEEditorCommon::AEEditorCommon(Context* context) :
    Application(context)
{

}

void AEEditorCommon::Start()
{
    Input* input = GetSubsystem<Input>();
    input->SetMouseVisible(true);

    Javascript* javascript = GetSubsystem<Javascript>();
    vm_ = javascript->InstantiateVM("MainVM");
    vm_->InitJSContext();

    jsapi_init_toolcore(vm_);

#ifdef ATOMIC_DOTNET
    jsapi_init_atomicnet(vm_);
#endif

}

void AEEditorCommon::Setup()
{

#ifdef ATOMIC_3D
    RegisterEnvironmentLibrary(context_);
#endif

    RegisterEditorComponentLibrary(context_);

#ifdef ATOMIC_DOTNET
    RegisterNETScriptLibrary(context_);
#endif

    // Register IPC system
    context_->RegisterSubsystem(new IPC(context_));

    context_->RegisterSubsystem(new ScriptSystem(context_));

    // Instantiate and register the Javascript subsystem
    Javascript* javascript = new Javascript(context_);
    context_->RegisterSubsystem(javascript);

    ToolEnvironment* env = new ToolEnvironment(context_);
    context_->RegisterSubsystem(env);

#ifdef ATOMIC_DEV_BUILD

    if (!env->InitFromJSON())
    {
        ErrorExit(ToString("Unable to initialize tool environment from %s", env->GetDevConfigFilename().CString()));
        return;
    }
#else

    env->InitFromPackage();

#endif

#ifdef ATOMIC_DOTNET

    // Instantiate and register the AtomicNET subsystem
    SharedPtr<NETCore> netCore (new NETCore(context_));
    context_->RegisterSubsystem(netCore);
    String netCoreErrorMsg;

    NETHost::SetCoreCLRFilesAbsPath(env->GetNETCoreCLRAbsPath());
    NETHost::SetCoreCLRTPAPaths(env->GetNETTPAPaths());
    NETHost::SetCoreCLRAssemblyLoadPaths(env->GetNETAssemblyLoadPaths());

    if (!netCore->Initialize(netCoreErrorMsg))
    {
        LOGERRORF("NetCore: Unable to initialize! %s", netCoreErrorMsg.CString());
        context_->RemoveSubsystem(NETCore::GetTypeStatic());
    }
    else
    {

    }
#endif

    ToolSystem* system = new ToolSystem(context_);
    context_->RegisterSubsystem(system);

}

void AEEditorCommon::Stop()
{

    context_->RemoveSubsystem<IPC>();

    vm_ = 0;
    context_->RemoveSubsystem<Javascript>();
    // make sure JSVM is really down and no outstanding refs
    // as if not, will hold on engine subsystems, which is bad
    assert(!JSVM::GetJSVM(0));

#ifdef ATOMIC_DOTNET
    NETCore* netCore = GetSubsystem<NETCore>();
    if (netCore)
    {
        netCore->Shutdown();
        context_->RemoveSubsystem<NETCore>();
    }
#endif
}

bool AEEditorCommon::CreateDefaultPreferences(String& path, JSONValue& prefs)
{
    // Note there is some duplication here with the editor's
    // TypeScript preference code, this is due to the preferences for
    // the editor window needing to be available at window creation time
    // It could be better to split this all out to a native, scriptable
    // preferences object

    LOGINFOF("Creating default Atomic Editor preferences: %s", path.CString());

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue& root = jsonFile->GetRoot();

    root.Clear();
    root["recentProjects"] = JSONArray();

    JSONValue editorWindow;
    editorWindow["x"] = 0;
    editorWindow["y"] = 0;
    editorWindow["width"] = 0;
    editorWindow["height"] = 0;
    editorWindow["monitor"] = 0;
    editorWindow["maximized"] = true;

    JSONValue playerWindow;
    playerWindow["x"] = 0;
    playerWindow["y"] = 0;
    playerWindow["width"] = 0;
    playerWindow["height"] = 0;
    playerWindow["monitor"] = 0;
    playerWindow["maximized"] = false;

    root["editorWindow"] = editorWindow;
    root["playerWindow"] = playerWindow;

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    if (!file->IsOpen())
    {
        LOGERRORF("Unable to open Atomic Editor preferences for writing: %s", path.CString());
        return false;
    }

    jsonFile->Save(*file, "   ");

    prefs = root;

    return true;
}

bool AEEditorCommon::ReadPreferences()
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    String path = fileSystem->GetAppPreferencesDir("AtomicEditor", "Preferences");
    path += "prefs.json";

    JSONValue prefs;

    if (!fileSystem->FileExists(path))
    {
        if (!CreateDefaultPreferences(path, prefs))
            return false;
    }
    else
    {
        SharedPtr<File> file(new File(context_, path, FILE_READ));
        SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

        if (!jsonFile->BeginLoad(*file))
        {
            file->Close();
            if (!CreateDefaultPreferences(path, prefs))
                return false;
        }
        else
        {
            prefs = jsonFile->GetRoot();
        }

    }

    if (!prefs.IsObject() || !prefs["editorWindow"].IsObject())
    {
        if (!CreateDefaultPreferences(path, prefs))
            return false;
    }

    JSONValue& editorWindow = prefs["editorWindow"];

    engineParameters_["WindowPositionX"] = editorWindow["x"].GetUInt();
    engineParameters_["WindowPositionY"] = editorWindow["y"].GetUInt();
    engineParameters_["WindowWidth"] = editorWindow["width"].GetUInt();
    engineParameters_["WindowHeight"] = editorWindow["height"].GetUInt();
    engineParameters_["WindowMaximized"] = editorWindow["maximized"].GetBool();

    return true;
}

}
