// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/FileSystem.h>
#include "../AEEditor.h"
#include "../Project/AEProject.h"
#include "../Project/ProjectUtils.h"
#include "License/AELicenseSystem.h"

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

#ifdef ATOMIC_PLATFORM_WINDOWS
    String bundleResources = fileSystem->GetProgramDir();
#else
    String bundleResources = fileSystem->GetAppBundleResourceFolder();
#endif

    String projectResources = project->GetResourcePath();
    String coreDataFolder = bundleResources + "CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

void BuildMac::Build(const String& buildPath)
{
    buildPath_ = buildPath + "/Mac-Build";

    Initialize();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

// BEGIN LICENSE MANAGEMENT
    LicenseSystem *licenseSystem = GetSubsystem<LicenseSystem>();
    if (licenseSystem->IsStandardLicense())
    {
        if (containsMDL_)
        {
            buildSystem->BuildComplete(AE_PLATFORM_MAC, buildPath_, false, true);
            return;
        }
    }
// END LICENSE MANAGEMENT

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

 #ifdef ATOMIC_PLATFORM_WINDOWS
    String buildSourceDir = fileSystem->GetProgramDir();
 #else
    String buildSourceDir = fileSystem->GetAppBundleResourceFolder();
 #endif

    buildSourceDir += "Deployment/MacOS/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath_);

    buildPath_ += "/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath_);

    fileSystem->CreateDir(buildPath_ + "/Contents");
    fileSystem->CreateDir(buildPath_ + "/Contents/MacOS");
    fileSystem->CreateDir(buildPath_ + "/Contents/Resources");

    String resourcePackagePath = buildPath_ + "/Contents/Resources/AtomicResources.pak";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildSourceDir + "/Contents/Resources/Atomic.icns", buildPath_ + "/Contents/Resources/Atomic.icns");

    fileSystem->Copy(buildSourceDir + "/Contents/Info.plist", buildPath_ + "/Contents/Info.plist");
    fileSystem->Copy(buildSourceDir + "/Contents/MacOS/AtomicPlayer", buildPath_ + "/Contents/MacOS/AtomicPlayer");

#ifdef ATOMIC_PLATFORM_OSX
    Vector<String> args;
    args.Push("+x");
    args.Push(buildPath_ + "/Contents/MacOS/AtomicPlayer");
    fileSystem->SystemRun("chmod", args);
#endif

    buildPath_ = buildPath + "/Mac-Build";    
    buildSystem->BuildComplete(AE_PLATFORM_MAC, buildPath_);

}

}
