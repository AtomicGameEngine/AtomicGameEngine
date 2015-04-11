
// before resource system exists so use rapidjson directly
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filestream.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "ToolEnvironment.h"

using namespace rapidjson;


namespace ToolCore
{

ToolEnvironment::ToolEnvironment(Context* context) : Object(context)
{

}

ToolEnvironment::~ToolEnvironment()
{

}

bool ToolEnvironment::InitFromJSON()
{

#ifndef ATOMIC_DEV_BUILD
    return false;
#else

    // make sure config path is initialized
    GetDevConfigFilename();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (!fileSystem->FileExists(devConfigFilename_))
        return false;

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

    //env->SetRootSourceDir("/Users/josh/Dev/atomic/AtomicGameEngine");
    //env->SetRootBuildDir("/Users/josh/Dev/atomic/AtomicGameEngine-build", true);

    return true;

#endif

}


const String& ToolEnvironment::GetDevConfigFilename()
{
    if (devConfigFilename_.Length())
        return devConfigFilename_;

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_OSX
    devConfigFilename_ = fileSystem->GetUserDocumentsDir() = ".atomicgameengine/devConfig.json";
#endif

    return devConfigFilename_;
}

void ToolEnvironment::SetRootSourceDir(const String& sourceDir)
{
    rootSourceDir_ = AddTrailingSlash(sourceDir);
    resourceCoreDataDir_ = rootSourceDir_ + "Data/AtomicPlayer/Resources/CoreData";
}

void ToolEnvironment::SetRootBuildDir(const String& buildDir, bool setBinaryPaths)
{
    rootBuildDir_ = AddTrailingSlash(buildDir);

    if (setBinaryPaths)
    {
#ifdef ATOMIC_PLATFORM_OSX
        playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";
#endif
    }

}

}
