//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/FileSystem.h>

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

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Starting Mac Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    Initialize();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

    String appSrcPath = tenv->GetPlayerAppFolder();

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

    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Mac Deployment Complete</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    buildPath_ = buildPath + "/Mac-Build";
    buildSystem->BuildComplete(PLATFORMID_MAC, buildPath_);

}

}
