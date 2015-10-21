//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

// before resource system exists so use rapidjson directly
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filestream.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "ToolEnvironment.h"

using namespace rapidjson;


namespace ToolCore
{

ToolEnvironment::ToolEnvironment(Context* context) : Object(context),
    toolPrefs_(new ToolPrefs(context))
{

}

ToolEnvironment::~ToolEnvironment()
{

}

bool ToolEnvironment::InitFromPackage()
{
    toolPrefs_->Load();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_WINDOWS
	editorBinary_ = fileSystem->GetProgramDir() + "AtomicEditor.exe";
    String resourcesDir = fileSystem->GetProgramDir() + "Resources/";
#else
    editorBinary_ = fileSystem->GetProgramDir() + "AtomicEditor";
    String resourcesDir = GetPath(RemoveTrailingSlash(fileSystem->GetProgramDir())) + "Resources/";
#endif

    //TODO: move this to deployment stuff
    playerAppFolder_ = resourcesDir + "ToolData/Deployment/MacOS/AtomicPlayer.app/";
    playerBinary_ = resourcesDir + "ToolData/Deployment/Windows/x64/AtomicPlayer.exe";

    resourceCoreDataDir_ = resourcesDir + "CoreData";
    resourcePlayerDataDir_ = resourcesDir + "PlayerData";

    toolDataDir_ =  resourcesDir + "ToolData/";

    // AtomicNET
    netAssemblyLoadPaths_ = GetNativePath(ToString("%sAtomicNET/Windows/Atomic/", resourcesDir.CString()));

#ifdef ATOMIC_PLATFORM_WINDOWS
    netCoreCLRAbsPath_ = GetNativePath(ToString("%sAtomicNET/Windows/x64/", resourcesDir.CString()));
    netTPAPaths_ = ToString("%sAtomicNET/Windows/Atomic/TPA/", resourcesDir.CString());
#else
    netCoreCLRAbsPath_ = GetNativePath(ToString("%sAtomicNET/Windows/x64/", resourcesDir.CString()));
    netTPAPaths_ = ToString("%sAtomicNET/Windows/Atomic/TPA/", resourcesDir.CString());
#endif

    return true;
}

bool ToolEnvironment::InitFromJSON(bool atomicTool)
{

    toolPrefs_->Load();

    // make sure config path is initialized
    GetDevConfigFilename();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (atomicTool || !fileSystem->FileExists(devConfigFilename_))
    {
        // default to build directories

        SetRootSourceDir(ATOMIC_ROOT_SOURCE_DIR);
        SetRootBuildDir(ATOMIC_ROOT_BUILD_DIR, true);

        netAssemblyLoadPaths_ = GetNativePath(ToString("%s/Artifacts/AtomicNET/", ATOMIC_ROOT_SOURCE_DIR));
        netAtomicNETEngineAssemblyPath_ = ToString("%s/Artifacts/AtomicNET/", ATOMIC_ROOT_SOURCE_DIR);

#ifdef ATOMIC_PLATFORM_WINDOWS
        netCoreCLRAbsPath_ = GetNativePath(ToString("%s/Submodules/CoreCLR/Windows/Release/x64/", ATOMIC_ROOT_SOURCE_DIR));
#else
        netCoreCLRAbsPath_ = GetNativePath(ToString("%s/Submodules/CoreCLR/MacOSX/Debug/x64/", ATOMIC_ROOT_SOURCE_DIR));
#endif

        netTPAPaths_ = ToString("%s/Artifacts/AtomicNET/TPA/", ATOMIC_ROOT_SOURCE_DIR);

        return true;
    }

    File jsonFile(context_, devConfigFilename_);

    if (!jsonFile.IsOpen())
        return false;

    String json;
    jsonFile.ReadText(json);

    if (!json.Length())
        return false;

    rapidjson::Document document;
    if (document.Parse<0>(json.CString()).HasParseError())
    {
        return false;
    }

    const Value::Member* rootSourceDir = document.FindMember("rootSourceDir");
    if (rootSourceDir && rootSourceDir->value.IsString())
        SetRootSourceDir(rootSourceDir->value.GetString());
    else
        return false;

    const Value::Member* rootBuildDir = document.FindMember("rootBuildDir");
    if (rootBuildDir && rootBuildDir->value.IsString())
        SetRootBuildDir(rootBuildDir->value.GetString(), true);
    else
        return false;


    return true;

}


const String& ToolEnvironment::GetDevConfigFilename()
{
    if (devConfigFilename_.Length())
        return devConfigFilename_;

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
    devConfigFilename_ = fileSystem->GetUserDocumentsDir() + ".atomicgameengine/toolEnv.json";
#elif ATOMIC_PLATFORM_WINDOWS
    devConfigFilename_ = fileSystem->GetUserDocumentsDir() + "AtomicGameEngine/toolEnv.json";
#endif

    return devConfigFilename_;
}

void ToolEnvironment::SetRootSourceDir(const String& sourceDir)
{
    rootSourceDir_ = AddTrailingSlash(sourceDir);
    resourceCoreDataDir_ = rootSourceDir_ + "Resources/CoreData";
    resourcePlayerDataDir_ = rootSourceDir_ + "Resources/PlayerData";
    resourceEditorDataDir_ = rootSourceDir_ + "Resources/EditorData";

    toolDataDir_ = rootSourceDir_ + "Data/AtomicEditor/";
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

#endif
    }

}

String ToolEnvironment::GetIOSDeployBinary()
{
    return GetToolDataDir() + "Deployment/IOS/ios-deploy/ios-deploy";
}

void ToolEnvironment::Dump()
{
    LOGINFOF("Root Source Dir: %s", rootSourceDir_.CString());
    LOGINFOF("Root Build Dir: %s", rootBuildDir_.CString());

    LOGINFOF("Core Resource Dir: %s", resourceCoreDataDir_.CString());
    LOGINFOF("Player Resource Dir: %s", resourcePlayerDataDir_.CString());
    LOGINFOF("Editor Resource Dir: %s", resourceEditorDataDir_.CString());

    LOGINFOF("Editor Binary: %s", editorBinary_.CString());
    LOGINFOF("Player Binary: %s", playerBinary_.CString());
    LOGINFOF("Tool Binary: %s", toolBinary_.CString());


    LOGINFOF("Tool Data Dir: %s", toolDataDir_.CString());

    LOGINFOF("Deployment Data Dir: %s", deploymentDataDir_.CString());

    LOGINFOF("Dev Config File: %s", devConfigFilename_.CString());

}

}
