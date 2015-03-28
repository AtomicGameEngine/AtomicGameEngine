// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/IO/FileSystem.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "BuildMac.h"
#include "BuildSystem.h"

namespace ToolCore
{

BuildMac::BuildMac(Context * context, Project *project) : BuildBase(context, project)
{

}

BuildMac::~BuildMac()
{

}

void BuildMac::Initialize()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    String dataPath = tsystem->GetDataPath();
    String projectResources = project->GetResourcePath();
    String coreDataFolder = dataPath + "CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

void BuildMac::Build(const String& buildPath)
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    Initialize();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

    String dataPath = tsystem->GetDataPath();

    String appSrcPath = dataPath + "Atomic/Deployment/MacOS/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath_);

    buildPath_ += "/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath_);

    fileSystem->CreateDir(buildPath_ + "/Contents");
    fileSystem->CreateDir(buildPath_ + "/Contents/MacOS");
    fileSystem->CreateDir(buildPath_ + "/Contents/Resources");

    String resourcePackagePath = buildPath_ + "/Contents/Resources/AtomicResources.pak";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(appSrcPath + "/Contents/Resources/Atomic.icns", buildPath_ + "/Contents/Resources/Atomic.icns");

    fileSystem->Copy(appSrcPath + "/Contents/Info.plist", buildPath_ + "/Contents/Info.plist");
    fileSystem->Copy(appSrcPath + "/Contents/MacOS/AtomicPlayer", buildPath_ + "/Contents/MacOS/AtomicPlayer");

#ifdef ATOMIC_PLATFORM_OSX
    Vector<String> args;
    args.Push("+x");
    args.Push(buildPath_ + "/Contents/MacOS/AtomicPlayer");
    fileSystem->SystemRun("chmod", args);
#endif

    buildPath_ = buildPath + "/Mac-Build";    
    buildSystem->BuildComplete(PLATFORMID_MAC, buildPath_);

}

}
