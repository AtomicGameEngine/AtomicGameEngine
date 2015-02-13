// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#include "AtomicEditor.h"

#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Main.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>
#include <Atomic/DebugNew.h>

#include <Atomic/Engine/Console.h>
#include <Atomic/Engine/DebugHud.h>

#include <Atomic/Input/Input.h>

#include <Atomic/UI/TBUI.h>

#include <Atomic/Javascript/Javascript.h>

#include <Atomic/Environment/Environment.h>
#include <Atomic/Graphics/Renderer.h>

#include "Project/ProjectUtils.h"
#include "Subprocess/AESubprocessSystem.h"
#include "Build/BuildSystem.h"
#include "License/AELicenseSystem.h"
#include "Net/CurlManager.h"

// just for testing, remove me
#include "Import/AEImportJSON.h"
#include "Import/AEJSONSceneProcess.h"

#include "AEEditor.h"

#include "AEApplication.h"

#include "Tools/External/AEExternalTooling.h"

DEFINE_APPLICATION_MAIN(AtomicEditor::AEApplication);

namespace AtomicEditor
{

AEApplication::AEApplication(Context* context) :
    Application(context)
{
}

void AEApplication::Start()
{
    // refactor this
    RegisterEnvironmenttLibrary(context_);

    // Get default style
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef __APPLE__    
    String editorResources = fileSystem->GetAppBundleResourceFolder() + "EditorResources/";
#else
    String editorResources = fileSystem->GetProgramDir() + "EditorResources/";
#endif    
    assert(fileSystem->DirExists(editorResources));
    cache->AddResourceDir(editorResources);

    // initialize after EditorResources set
    TBUI* tbui = GetSubsystem<TBUI>();
    tbui->Initialize();

    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create console
    Console* console = engine_->CreateConsole();
    console->SetDefaultStyle(xmlFile);
    console->GetBackground()->SetOpacity(0.8f);

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);

    Input* input = GetSubsystem<Input>();

    // The mouse isn't showing up on OSX until I tab, this is a hack and temp workaround
    input->SetMouseVisible(true);
    input->SetMouseVisible(false);
    input->SetMouseVisible(true);
    input->SetMouseVisible(false);
    input->SetMouseVisible(true);

    context_->RegisterSubsystem(new ProjectUtils(context_));
    context_->RegisterSubsystem(new Javascript(context_));
    context_->RegisterSubsystem(new SubprocessSystem(context_));
    context_->RegisterSubsystem(new ExternalTooling(context_));
    context_->RegisterSubsystem(new BuildSystem(context_));
    context_->RegisterSubsystem(new CurlManager(context_));

// BEGIN LICENSE MANAGEMENT
    context_->RegisterSubsystem(new LicenseSystem(context_));
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
}

void AEApplication::Setup()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();

    engineParameters_["WindowTitle"] = "AtomicEditor";
    engineParameters_["WindowResizable"] = true;
    engineParameters_["FullScreen"] = false;

#ifdef __APPLE__
    engineParameters_["ResourcePrefixPath"] = "../Resources";
#endif

    engineParameters_["LogName"] = filesystem->GetAppPreferencesDir("AtomicEditor", "Logs") + "AtomicEditor.log";

    // Show usage if not found
    if (false)//scriptFileName_.Empty())
    {
        ErrorExit("Usage: AtomicEditor <scriptfile> [options]\n\n"
            "The script file should implement the function void Start() for initializing the "
            "application and subscribing to all necessary events, such as the frame update.\n"
            #ifndef WIN32
            "\nCommand line options:\n"
            "-x <res>     Horizontal resolution\n"
            "-y <res>     Vertical resolution\n"
            "-m <level>   Enable hardware multisampling\n"
            "-v           Enable vertical sync\n"
            "-t           Enable triple buffering\n"
            "-w           Start in windowed mode\n"
            "-s           Enable resizing when in windowed mode\n"
            "-q           Enable quiet mode which does not log to standard output stream\n"
            "-b <length>  Sound buffer length in milliseconds\n"
            "-r <freq>    Sound mixing frequency in Hz\n"
            "-p <paths>   Resource path(s) to use, separated by semicolons\n"
            "-ap <paths>  Autoload resource path(s) to use, seperated by semicolons\n"
            "-log <level> Change the log level, valid 'level' values are 'debug', 'info', 'warning', 'error'\n"
            "-ds <file>   Dump used shader variations to a file for precaching\n"
            "-mq <level>  Material quality level, default 2 (high)\n"
            "-tq <level>  Texture quality level, default 2 (high)\n"
            "-tf <level>  Texture filter mode, default 2 (trilinear)\n"
            "-af <level>  Texture anisotropy level, default 4. Also sets anisotropic filter mode\n"
            "-flushgpu    Flush GPU command queue each frame. Effective only on Direct3D9\n"
            "-borderless  Borderless window mode\n"
            "-headless    Headless mode. No application window will be created\n"
            "-landscape   Use landscape orientations (iOS only, default)\n"
            "-portrait    Use portrait orientations (iOS only)\n"
            "-prepass     Use light pre-pass rendering\n"
            "-deferred    Use deferred rendering\n"
            "-lqshadows   Use low-quality (1-sample) shadow filtering\n"
            "-noshadows   Disable shadow rendering\n"
            "-nolimit     Disable frame limiter\n"
            "-nothreads   Disable worker threads\n"
            "-nosound     Disable sound output\n"
            "-noip        Disable sound mixing interpolation\n"
            "-sm2         Force SM2.0 rendering\n"
            "-touch       Touch emulation on desktop platform\n"
            #endif
        );
    }
    else
    {
        // Use the script file name as the base name for the log file

    }
}

void AEApplication::Stop()
{
    context_->RemoveSubsystem<Editor>();
}

}
