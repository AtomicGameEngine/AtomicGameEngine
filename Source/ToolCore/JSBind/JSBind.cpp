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

    String modulesFolder = "Build/Source/Generated/" + platform_ + "/CSharp/Packages/";
    modulesFolder += package_->GetName() + "/";

    String outputFolder = sourceRootFolder_ + "/" + modulesFolder;

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (!fs->CreateDirs(sourceRootFolder_, modulesFolder) || !fs->DirExists(outputFolder))
    {
        String error = "Unable to create bindings output folder: " + outputFolder;
        ErrorExit(error.CString());
    }

    destScriptFolder_ = outputFolder;
    destNativeFolder_ = outputFolder;

    CSPackageWriter writer(package_);
    package_->GenerateSource(writer);

    return true;
}


bool JSBind::GenerateJavaScriptBindings()
{
    String modulesFolder = "Build/Source/Generated/" + platform_ + "/Javascript/Packages/";
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
