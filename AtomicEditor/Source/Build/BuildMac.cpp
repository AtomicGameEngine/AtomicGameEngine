// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#include "AtomicEditor.h"
#include <Atomic/IO/FileSystem.h>
#include "../AEEditor.h"
#include "../Project/AEProject.h"
#include "../Project/ProjectUtils.h"

#include "BuildMac.h"
#include "BuildSystem.h"

namespace AtomicEditor
{

BuildMac::BuildMac(Context * context) : BuildBase(context)
{

}

BuildMac::~BuildMac()
{

}

void BuildMac::Initialize()
{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    String bundleResources = fileSystem->GetAppBundleResourceFolder();

    String projectResources = project->GetResourcePath();
    String dataFolder = bundleResources + "Data/";
    String coreDataFolder = bundleResources + "CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(dataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

void BuildMac::Build(const String& buildPath)
{
    buildPath_ = buildPath;

    Initialize();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    String buildSourceDir = fileSystem->GetAppBundleResourceFolder();
    buildSourceDir += "Deployment/MacOS/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath);
    fileSystem->CreateDir(buildPath + "/Contents");
    fileSystem->CreateDir(buildPath + "/Contents/MacOS");
    fileSystem->CreateDir(buildPath + "/Contents/Resources");

    String resourcePackagePath = buildPath + "/Contents/Resources/AtomicResources.pak";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildSourceDir + "/Contents/Resources/Atomic.icns", buildPath + "/Contents/Resources/Atomic.icns");

    fileSystem->Copy(buildSourceDir + "/Contents/Info.plist", buildPath + "/Contents/Info.plist");    
    fileSystem->Copy(buildSourceDir + "/Contents/MacOS/AtomicPlayer", buildPath + "/Contents/MacOS/AtomicPlayer");

    Vector<String> args;
    args.Push("+x");
    args.Push(buildPath + "/Contents/MacOS/AtomicPlayer");
    fileSystem->SystemRun("chmod", args);

    ProjectUtils* utils = GetSubsystem<ProjectUtils>();
    utils->RevealInFinder(GetPath(buildPath));


    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete();


}

}
