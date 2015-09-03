// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"

#include "AndroidProjectGenerator.h"

#include "BuildSystem.h"
#include "BuildAndroid.h"

namespace ToolCore
{

BuildAndroid::BuildAndroid(Context* context, Project* project) : BuildBase(context, project)
{

}

BuildAndroid::~BuildAndroid()
{

}

void BuildAndroid::Initialize()
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
void BuildAndroid::Build(const String& buildPath)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    Initialize();

    //generate manifest file
    String manifest;
    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        BuildResourceEntry* entry = resourceEntries_[i];
        manifest += entry->packagePath_;
        if ( i != resourceEntries_.Size() - 1)
            manifest += ";";
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

    String buildSourceDir = tenv->GetToolDataDir();

    String androidProject = buildSourceDir + "Deployment/Android";

    // Copy the base android project
    fileSystem->CopyDir(androidProject, buildPath_);

    Vector<String> defaultResourcePaths;
    GetDefaultResourcePaths(defaultResourcePaths);
    String projectResources = project->GetResourcePath();

    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        fileSystem->CopyDir(defaultResourcePaths[i], buildPath_ + "/assets/" + GetFileName(RemoveTrailingSlash(defaultResourcePaths[i])));
    }

    fileSystem->CopyDir(project->GetProjectPath() + "Cache/", buildPath_ + "/assets/Cache");
    fileSystem->CopyDir(projectResources, buildPath_ + "/assets/AtomicResources");

    // write the manifest
    SharedPtr<File> mfile(new File(context_, buildPath_ + "/assets/AtomicManifest", FILE_WRITE));
    mfile->WriteString(manifest);
    mfile->Close();


    AndroidProjectGenerator gen(context_);
    gen.SetBuildPath(buildPath_);

    if (!gen.Generate())
    {
        return;
    }

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete(PLATFORMID_ANDROID, buildPath_);

    //fileSystem->SystemCommandAsync("/Applications/Firefox.app/Contents/MacOS/firefox");

}

}
