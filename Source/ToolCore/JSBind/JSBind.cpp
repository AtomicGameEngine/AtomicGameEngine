//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/FileSystem.h>

#include "JSBPackage.h"
#include "JSBind.h"

#include "JavaScript/JSPackageWriter.h"
#include "CSharp/CSPackageWriter.h"

namespace ToolCore
{

JSBind::JSBind(Context* context) : Object(context),
    package_(new JSBPackage(context))
{

}

JSBind::~JSBind()
{

}

bool JSBind::LoadPackage(const String& sourceRootFolder, const String& packageFolder, const String& platform)
{
    sourceRootFolder_ = sourceRootFolder;
    packageFolder_ = packageFolder;
    platform_ = platform;

    package_->Load(sourceRootFolder_ + packageFolder_);

    return true;
}

bool JSBind::GenerateCSharpBindings()
{

    String modulesFolder = "Artifacts/Build/Source/Generated/" + platform_ + "/CSharp/Packages/";
    modulesFolder += package_->GetName() + "/";

    String nativeOutputFolder = sourceRootFolder_ + "/" + modulesFolder + "Native/";
    String managedOutputFolder = sourceRootFolder_ + "/" + modulesFolder + "Managed/";

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (!fs->CreateDirs(sourceRootFolder_, modulesFolder + "Native/") || !fs->DirExists(nativeOutputFolder))
    {
        String error = "Unable to create bindings native output folder: " + nativeOutputFolder;
        ErrorExit(error.CString());
    }

    if (!fs->CreateDirs(sourceRootFolder_, modulesFolder + "Managed/") || !fs->DirExists(managedOutputFolder))
    {
        String error = "Unable to create bindings managed output folder: " + managedOutputFolder;
        ErrorExit(error.CString());
    }


    destScriptFolder_ = managedOutputFolder;
    destNativeFolder_ = nativeOutputFolder;

    CSPackageWriter writer(package_);
    package_->GenerateSource(writer);

    return true;
}


bool JSBind::GenerateJavaScriptBindings()
{
    String modulesFolder = "Artifacts/Build/Source/Generated/" + platform_ + "/Javascript/Packages/";
    modulesFolder += package_->GetName() + "/";

    String outputFolder = sourceRootFolder_ + "/" + modulesFolder;

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (!fs->CreateDirs(sourceRootFolder_, modulesFolder) || !fs->DirExists(outputFolder))
    {
        String error = "Unable to create bindings output folder: " + outputFolder;
        ErrorExit(error.CString());
    }

    destScriptFolder_ = String::EMPTY;
    destNativeFolder_ = outputFolder;

    JSPackageWriter writer(package_);
    package_->GenerateSource(writer);

    return true;
}

}
