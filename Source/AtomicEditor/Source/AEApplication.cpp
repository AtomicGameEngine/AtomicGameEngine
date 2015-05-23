// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Resource/XMLFile.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>
#include <Atomic/DebugNew.h>

#include <Atomic/Engine/Console.h>
#include <Atomic/Engine/DebugHud.h>

#include <Atomic/Input/Input.h>

#include <Atomic/UI/UI.h>
#include <Atomic/Environment/Environment.h>
#include <Atomic/Graphics/Renderer.h>

#include <Atomic/IPC/IPC.h>

#include "AEEditorStrings.h"
#include "AEEditorShortcuts.h"
#include "Project/ProjectUtils.h"
#include "Subprocess/AESubprocessSystem.h"
#include "Build/BuildSystem.h"
#include "License/AELicenseSystem.h"
#include "License/AEVersionCheck.h"
#include "Net/CurlManager.h"

// just for testing, remove me
#include "Import/AEImportJSON.h"
#include "Import/AEJSONSceneProcess.h"

#include "AEEditor.h"
#include "AEPreferences.h"
#include "AEApplication.h"

#include "Tools/External/AEExternalTooling.h"

#include <AtomicJS/Javascript/Javascript.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>

using namespace ToolCore;

namespace AtomicEditor
{

AEApplication::AEApplication(Context* context) :
    Application(context)
{

}

void AEApplication::Start()
{
    // refactor this
    RegisterEnvironmentLibrary(context_);

    ToolSystem* tsystem = new ToolSystem(context_);
    context_->RegisterSubsystem(tsystem);

    Engine* engine = GetSubsystem<Engine>();
    engine->SetAutoExit(false);

    // Register IPC system
    context_->RegisterSubsystem(new IPC(context_));

    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

#ifndef ATOMIC_DEV_BUILD

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef __APPLE__    
    String editorResources = fileSystem->GetAppBundleResourceFolder() + "EditorData.pak";
#else
    String editorResources = fileSystem->GetProgramDir() + "EditorData.pak";
#endif

    assert(fileSystem->FileExists(editorResources));
    cache->AddPackageFile(editorResources);

#endif

    // initialize after EditorResources set
    UI* tbui = GetSubsystem<UI>();

    tbui->Initialize("AtomicEditor/resources/language/lng_en.tb.txt");
    tbui->LoadSkin("AtomicEditor/resources/default_skin/skin.tb.txt", "AtomicEditor/editor/skin/skin.tb.txt");
    tbui->AddFont("AtomicEditor/resources/vera.ttf", "Vera");
    tbui->AddFont("AtomicEditor/resources/MesloLGS-Regular.ttf", "Monaco");
    tbui->SetDefaultFont("Vera", 12);

    Input* input = GetSubsystem<Input>();
    input->SetMouseVisible(true);

 #ifdef ATOMIC_PLATFORM_WINDOWS
    // on windows maximize, otherwise window does funky thing with "restore down"
    // todo: better way of handling this?  Windows 8 restore down goes below taskbar :(
    Graphics* graphics = GetSubsystem<Graphics>();
    IntVector2 resolution = graphics->GetDesktopResolution();
    int height = graphics->GetHeight();
    if ( Abs(height - resolution.y_) < 128)
        GetSubsystem<Graphics>()->Maximize();
 #endif

    context_->RegisterSubsystem(new EditorStrings(context_));
    context_->RegisterSubsystem(new EditorShortcuts(context_));
    context_->RegisterSubsystem(new ProjectUtils(context_));
    context_->RegisterSubsystem(new Javascript(context_));
    context_->RegisterSubsystem(new SubprocessSystem(context_));
    context_->RegisterSubsystem(new ExternalTooling(context_));
    context_->RegisterSubsystem(new BuildSystem(context_));
    context_->RegisterSubsystem(new CurlManager(context_));

// BEGIN LICENSE MANAGEMENT
    context_->RegisterSubsystem(new LicenseSystem(context_));
    context_->RegisterSubsystem(new VersionCheck(context_));
// END LICENSE MANAGEMENT

    Editor* editor = new Editor(context_);
    context_->RegisterSubsystem(editor);

#ifdef ATOMIC_PLATFORM_OSX
    editor->RequestPlatformChange(AE_PLATFORM_MAC);
#else
    editor->RequestPlatformChange(AE_PLATFORM_WINDOWS);
#endif

    bool process = false;
    if (process)
    {
        SharedPtr<JSONSceneImporter> jimporter;
        jimporter = new JSONSceneImporter(context_);
        jimporter->Import("/Users/josh/Desktop/Blah.js");

        SharedPtr<JSONSceneProcess> sceneProcess;
        sceneProcess = new JSONSceneProcess(context_, jimporter);
        sceneProcess->Process();
        sceneProcess->Write();
    }

    if (cmdLineProjectFile_.Length())
    {
        editor->LoadProject(cmdLineProjectFile_);
    }
}

void AEApplication::Setup()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();

    ToolEnvironment* env = new ToolEnvironment(context_);
    context_->RegisterSubsystem(env);

#ifdef ATOMIC_DEV_BUILD

    if (!env->InitFromJSON())
    {
        ErrorExit(ToString("Unable to initialize tool environment from %s", env->GetDevConfigFilename().CString()));
        return;
    }

#endif

    const Vector<String>& arguments = GetArguments();

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1 && arguments[i][0] == '-')
        {
            String argument = arguments[i].Substring(1).ToLower();
            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            if (argument == "-project" && value.Length())
            {
                Vector<String> projectFiles;
                filesystem->ScanDir(projectFiles, value, "*.atomic", SCAN_FILES, false);
                if (!projectFiles.Size())
                {
                    ErrorExit(ToString("No .atomic project file in %s", value.CString()));
                    return;
                }
                else if (projectFiles.Size() > 1)
                {
                    ErrorExit(ToString("Multiple .atomic project files found in %s", value.CString()));
                    return;
                }

                cmdLineProjectFile_ = value + "/" + projectFiles[0];

            }
        }
    }

    engineParameters_["WindowTitle"] = "AtomicEditor";
    engineParameters_["WindowResizable"] = true;
    engineParameters_["FullScreen"] = false;

    AEPreferences::StartupPreferences prefs;
    if (AEPreferences::ReadStartupPrefs(context_, prefs))
    {
        engineParameters_["WindowWidth"] = prefs.windowWidth;
        engineParameters_["WindowHeight"] = prefs.windowHeight;
    }

#ifdef ATOMIC_DEV_BUILD
    engineParameters_["ResourcePrefixPath"] = "";
    String resourcePaths = env->GetCoreDataDir() + ";" +  env->GetEditorDataDir();
    engineParameters_["ResourcePaths"] = resourcePaths;
#else

#ifdef __APPLE__
    engineParameters_["ResourcePrefixPath"] = "../Resources";
#endif

#endif // ATOMIC_DEV_BUILD

#ifndef __APPLE__
    engineParameters_["WindowIcon"] = "Images/AtomicLogo32.png";
#endif

    engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicEditor", "Logs") + "AtomicEditor.log";

}

void AEApplication::Stop()
{
    context_->RemoveSubsystem<Editor>();
}

}
