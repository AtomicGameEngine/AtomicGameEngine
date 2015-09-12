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

namespace ToolCore
{

JSBind::JSBind(Context* context) : Object(context)
{

}

JSBind::~JSBind()
{

}

bool JSBind::GenerateBindings(const String& sourceRootFolder, const String& packageFolder, const String& platform)
{
    sourceRootFolder_ = sourceRootFolder;
    packageFolder_ = packageFolder;
    platform_ = platform;

    SharedPtr<JSBPackage> package (new JSBPackage(context_));

    package->Load(sourceRootFolder_ + packageFolder_);

    String modulesFolder = "Build/Source/Generated/" + platform + "/Javascript/Packages/";
    modulesFolder += package->GetName() + "/";

    String outputFolder = sourceRootFolder + "/" + modulesFolder;

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (!fs->CreateDirs(sourceRootFolder, modulesFolder) || !fs->DirExists(outputFolder))
    {
        String error = "Unable to create bindings output folder: " + outputFolder;
        ErrorExit(error.CString());
    }

    package->GenerateSource(outputFolder);

    return true;
}

}
