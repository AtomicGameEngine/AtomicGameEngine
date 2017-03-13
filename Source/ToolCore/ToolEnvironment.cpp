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

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "ToolEnvironment.h"


namespace ToolCore
{

bool ToolEnvironment::bootstrapping_ = false;

ToolEnvironment::ToolEnvironment(Context* context) : Object(context),
    cli_(false),
    toolPrefs_(new ToolPrefs(context))
{

}

ToolEnvironment::~ToolEnvironment()
{

}

bool ToolEnvironment::InitFromDistribution()
{
    toolPrefs_->Load();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_WINDOWS
    editorBinary_ = fileSystem->GetProgramDir() + "AtomicEditor.exe";
    String resourcesDir = fileSystem->GetProgramDir() + "Resources/";
    playerBinary_ = resourcesDir + "ToolData/Deployment/Windows/x64/AtomicPlayer.exe";
#elif ATOMIC_PLATFORM_LINUX
    editorBinary_ = fileSystem->GetProgramDir() + "AtomicEditor";
    String resourcesDir = fileSystem->GetProgramDir() + "Resources/";
    playerBinary_ = resourcesDir + "ToolData/Deployment/Linux/AtomicPlayer";
#else
    editorBinary_ = fileSystem->GetProgramDir() + "AtomicEditor";
    String resourcesDir = GetPath(RemoveTrailingSlash(fileSystem->GetProgramDir())) + "Resources/";
    playerAppFolder_ = resourcesDir + "ToolData/Deployment/MacOS/AtomicPlayer.app/";
#endif

    resourceCoreDataDir_ = resourcesDir + "CoreData";
    resourcePlayerDataDir_ = resourcesDir + "PlayerData";

    toolDataDir_ =  resourcesDir + "ToolData/";

    // AtomicNET

#ifdef ATOMIC_DEBUG
    String config = "Debug";
#else
    String config = "Release";
#endif

    atomicNETRootDir_ = resourcesDir + "ToolData/AtomicNET/";
    atomicNETCoreAssemblyDir_ = atomicNETRootDir_ + config + "/";

#ifdef ATOMIC_PLATFORM_OSX
    monoExecutableDir_ = "/Library/Frameworks/Mono.framework/Versions/Current/Commands/";
    atomicNETNuGetBinary_ = monoExecutableDir_ + "nuget";
#endif

    return true;
}

bool ToolEnvironment::Initialize(bool cli)
{
    bool result = true;

    cli_ = cli;
    toolPrefs_->Load();

#ifdef ATOMIC_DEV_BUILD

    SetRootSourceDir(ATOMIC_ROOT_SOURCE_DIR);
    SetRootBuildDir(ATOMIC_ROOT_BUILD_DIR, true);

#else

    if (!bootstrapping_)
    {
        result = InitFromDistribution();

    }
    else
    {
        SetRootSourceDir(ATOMIC_ROOT_SOURCE_DIR);
        SetRootBuildDir(ATOMIC_ROOT_BUILD_DIR, true);

    }
#endif

    return result;

}

void ToolEnvironment::SetRootSourceDir(const String& sourceDir)
{
    rootSourceDir_ = AddTrailingSlash(sourceDir);
    resourceCoreDataDir_ = rootSourceDir_ + "Resources/CoreData";
    resourcePlayerDataDir_ = rootSourceDir_ + "Resources/PlayerData";
    resourceEditorDataDir_ = rootSourceDir_ + "Resources/EditorData";
    toolDataDir_ = rootSourceDir_ + "Data/AtomicEditor/";

    // AtomicNET

#ifdef ATOMIC_DEBUG
    String config = "Debug";
#else
    String config = "Release";
#endif

    atomicNETRootDir_ = rootSourceDir_ + "Artifacts/AtomicNET/";
    atomicNETCoreAssemblyDir_ = rootSourceDir_ + "Artifacts/AtomicNET/" + config + "/";

#if defined ATOMIC_PLATFORM_WINDOWS || defined ATOMIC_PLATFORM_LINUX
    atomicNETNuGetBinary_ = ToString("%sBuild/Managed/nuget/nuget.exe", rootSourceDir_.CString());
#endif

#ifdef ATOMIC_PLATFORM_OSX
    monoExecutableDir_ = "/Library/Frameworks/Mono.framework/Versions/Current/Commands/";
    atomicNETNuGetBinary_ = monoExecutableDir_ + "nuget";
#endif

}

void ToolEnvironment::SetRootBuildDir(const String& buildDir, bool setBinaryPaths)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    rootBuildDir_ = AddTrailingSlash(buildDir);


    if (setBinaryPaths)
    {
#ifdef ATOMIC_PLATFORM_WINDOWS

#ifdef _DEBUG
        playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/Application/Debug/AtomicPlayer.exe";
        editorBinary_ = rootBuildDir_ + "Source/AtomicEditor/Debug/AtomicEditor.exe";
#else
        playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/Application/Release/AtomicPlayer.exe";
        editorBinary_ = rootBuildDir_ + "Source/AtomicEditor/Release/AtomicEditor.exe";
#endif

        // some build tools like ninja don't use Release/Debug folders
        if (!fileSystem->FileExists(playerBinary_))
                playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/Application/AtomicPlayer.exe";
        if (!fileSystem->FileExists(editorBinary_))
                editorBinary_ = rootBuildDir_ + "Source/AtomicEditor/AtomicEditor.exe";

        playerAppFolder_ = rootSourceDir_ + "Data/AtomicEditor/Deployment/MacOS/AtomicPlayer.app";

#elif ATOMIC_PLATFORM_OSX

#ifdef ATOMIC_XCODE
        playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/" + CMAKE_INTDIR + "/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";
        editorBinary_ = rootBuildDir_ + "Source/AtomicEditor/" + CMAKE_INTDIR + "/AtomicEditor.app/Contents/MacOS/AtomicEditor";
#else
        playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/Application/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";
        playerAppFolder_ = rootBuildDir_ + "Source/AtomicPlayer/Application/AtomicPlayer.app/";
        editorBinary_ = rootBuildDir_ + "Source/AtomicEditor/AtomicEditor.app/Contents/MacOS/AtomicEditor";
#endif

#else
        playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/Application/AtomicPlayer";
        editorBinary_ = rootBuildDir_ + "Source/AtomicEditor/AtomicEditor";

#endif
    }

}

String ToolEnvironment::GetIOSDeployBinary()
{
    return GetToolDataDir() + "Deployment/IOS/ios-deploy/ios-deploy";
}

void ToolEnvironment::Dump()
{
    ATOMIC_LOGINFOF("Root Source Dir: %s", rootSourceDir_.CString());
    ATOMIC_LOGINFOF("Root Build Dir: %s", rootBuildDir_.CString());

    ATOMIC_LOGINFOF("Core Resource Dir: %s", resourceCoreDataDir_.CString());
    ATOMIC_LOGINFOF("Player Resource Dir: %s", resourcePlayerDataDir_.CString());
    ATOMIC_LOGINFOF("Editor Resource Dir: %s", resourceEditorDataDir_.CString());

    ATOMIC_LOGINFOF("Editor Binary: %s", editorBinary_.CString());
    ATOMIC_LOGINFOF("Player Binary: %s", playerBinary_.CString());
    ATOMIC_LOGINFOF("Tool Binary: %s", toolBinary_.CString());


    ATOMIC_LOGINFOF("Tool Data Dir: %s", toolDataDir_.CString());

    ATOMIC_LOGINFOF("Deployment Data Dir: %s", deploymentDataDir_.CString());

}

}
