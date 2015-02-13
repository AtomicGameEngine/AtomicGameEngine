// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

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
    String bundleResources = fileSystem->GetProgramDir();

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
    buildPath_ = buildPath;

    Initialize();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    String buildSourceDir = fileSystem->GetProgramDir();
    buildSourceDir += "Deployment/Win32";

    fileSystem->CreateDir(buildPath);
    fileSystem->CreateDir(buildPath + "/AtomicPlayer_Resources");

    String resourcePackagePath = buildPath + "/AtomicPlayer_Resources/AtomicResources.pak";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildSourceDir + "/AtomicPlayer.exe", buildPath + "/AtomicPlayer.exe");

    ProjectUtils* utils = GetSubsystem<ProjectUtils>();
    utils->RevealInFinder(GetPath(buildPath));

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete();

}

}
