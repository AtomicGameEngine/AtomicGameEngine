//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
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

    if (!package_->GenerateBindings(JSBPackage::CSHARP))
        return true;

    ATOMIC_LOGINFOF("Generating C# Bindings");

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
    if (!package_->GenerateBindings(JSBPackage::JAVASCRIPT))
        return true;

    ATOMIC_LOGINFOF("Generating JS Bindings");

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
