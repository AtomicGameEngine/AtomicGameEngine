// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/FileSystem.h>
#include "../AEEditor.h"
#include "../Project/AEProject.h"

#include "../Project/ProjectUtils.h"

#include "BuildSystem.h"
#include "BuildWindows.h"

namespace AtomicEditor
{

BuildWindows::BuildWindows(Context * context) : BuildBase(context)
{

}

BuildWindows::~BuildWindows()
{

}

void BuildWindows::Initialize()
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_WINDOWS
    String bundleResources = fileSystem->GetProgramDir();
#else
    String bundleResources = fileSystem->GetAppBundleResourceFolder();
#endif

    String projectResources = project->GetResourcePath();
    String dataFolder = bundleResources + "Data/";
    String coreDataFolder = bundleResources + "CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(dataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

void BuildWindows::Build(const String& buildPath)
{
    buildPath_ = buildPath + "/Windows-Build";

    Initialize();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

#ifdef ATOMIC_PLATFORM_WINDOWS
    String buildSourceDir = fileSystem->GetProgramDir();
#else
    String buildSourceDir = fileSystem->GetAppBundleResourceFolder();
#endif

    buildSourceDir += "Deployment/Win64";

    fileSystem->CreateDir(buildPath_);
    fileSystem->CreateDir(buildPath_ + "/AtomicPlayer_Resources");

    String resourcePackagePath = buildPath_ + "/AtomicPlayer_Resources/AtomicResources.pak";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildSourceDir + "/AtomicPlayer.exe", buildPath_ + "/AtomicPlayer.exe");

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete(AE_PLATFORM_WINDOWS, buildPath_);

}

}
