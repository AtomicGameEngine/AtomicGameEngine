// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Subprocess/SubprocessSystem.h"
#include "../Project/Project.h"

#include "AndroidProjectGenerator.h"

#include "BuildSystem.h"
#include "BuildEvents.h"
#include "BuildAndroid.h"

namespace ToolCore
{

BuildAndroid::BuildAndroid(Context* context, Project* project) : BuildBase(context, project)
{

}

BuildAndroid::~BuildAndroid()
{

}

void BuildAndroid::SendBuildFailure(const String& message)
{

    VariantMap buildError;
    buildError[BuildFailed::P_PLATFORMID] = PLATFORMID_ANDROID;
    buildError[BuildFailed::P_MESSAGE] = message;
    SendEvent(E_BUILDFAILED, buildError);

}

void BuildAndroid::RunAntDebug()
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    SubprocessSystem* subs = GetSubsystem<SubprocessSystem>();
    ToolPrefs* tprefs = tenv->GetToolPrefs();

    Poco::Process::Env env;

#ifdef ATOMIC_PLATFORM_OSX
    String antCommand = tprefs->GetAntPath();
    Vector<String> args;
    args.Push("debug");
#else
    // C:\ProgramData\Oracle\Java\javapath;
    Vector<String> args;
    String antCommand = "cmd";
    String antPath = prefs->GetAntPath() + "/ant.bat";
    env["JAVA_HOME"] = prefs->GetJDKRootPath().CString();
    // ant is a batch file on windows, so have to run with cmd /c
    args.Push("/c");
    args.Push("\"" + antPath + "\"");
    args.Push("debug");
#endif

    currentBuildPhase_ = AntBuildDebug;
    Subprocess* subprocess = subs->Launch(antCommand, args, buildPath_, env);

    if (!subprocess)
    {
        SendBuildFailure("BuildFailed::RunAntDebug");
        return;
    }

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "<color #D4FB79>Starting Android Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    //SubscribeToEvent(subprocess, E_SUBPROCESSCOMPLETE, HANDLER(BuildAndroid, HandleEvent));
    SubscribeToEvent(subprocess, E_SUBPROCESSOUTPUT, HANDLER(BuildBase, HandleSubprocessOutputEvent));

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
        SendBuildFailure(gen.GetErrorText());
        return;
    }

    RunAntDebug();

    //BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    //buildSystem->BuildComplete(PLATFORMID_ANDROID, buildPath_);

    //fileSystem->SystemCommandAsync("/Applications/Firefox.app/Contents/MacOS/firefox");

}

}
