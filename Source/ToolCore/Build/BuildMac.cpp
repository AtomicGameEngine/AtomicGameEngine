//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"

#include "BuildEvents.h"
#include "BuildSystem.h"
#include "BuildMac.h"

namespace ToolCore
{

BuildMac::BuildMac(Context * context, Project *project) : BuildBase(context, project, PLATFORMID_MAC)
{

}

BuildMac::~BuildMac()
{

}

void BuildMac::Initialize()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    Project* project = tsystem->GetProject();

    Vector<String> defaultResourcePaths;
    GetDefaultResourcePaths(defaultResourcePaths);
    String projectResources = project->GetResourcePath();

    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        AddResourceDir(defaultResourcePaths[i]);
    }

    // TODO: smart filtering of cache
    AddResourceDir(project->GetProjectPath() + "Cache/");
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

void BuildMac::Build(const String& buildPath)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    BuildLog("Starting Mac Deployment");

    Initialize();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    if (!BuildRemoveDirectory(buildPath_))
        return;

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    String appSrcPath = tenv->GetPlayerAppFolder();

    if (!BuildCreateDirectory(buildPath_))
        return;

    buildPath_ += "/AtomicPlayer.app";

    if (!BuildCreateDirectory(buildPath_))
        return;
    if (!BuildCreateDirectory(buildPath_ + "/Contents"))
        return;
    if (!BuildCreateDirectory(buildPath_ + "/Contents/MacOS"))
        return;
    if (!BuildCreateDirectory(buildPath_ + "/Contents/Resources"))
        return;

    String resourcePackagePath = buildPath_ + "/Contents/Resources/AtomicResources" + PAK_EXTENSION;
    GenerateResourcePackage(resourcePackagePath);

    if (!BuildCopyFile(appSrcPath + "/Contents/Resources/Atomic.icns", buildPath_ + "/Contents/Resources/Atomic.icns"))
        return;

    if (!BuildCopyFile(appSrcPath + "/Contents/Info.plist", buildPath_ + "/Contents/Info.plist"))
        return;

    if (!BuildCopyFile(appSrcPath + "/Contents/MacOS/AtomicPlayer", buildPath_ + "/Contents/MacOS/AtomicPlayer"))
        return;

#ifdef ATOMIC_PLATFORM_OSX
    Vector<String> args;
    args.Push("+x");
    args.Push(buildPath_ + "/Contents/MacOS/AtomicPlayer");
    fileSystem->SystemRun("chmod", args);
#endif

    BuildLog("Mac Deployment Complete");

    buildPath_ = buildPath + "/Mac-Build";
    buildSystem->BuildComplete(PLATFORMID_MAC, buildPath_);

}

}
