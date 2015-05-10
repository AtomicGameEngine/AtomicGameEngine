
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
    {
        // default to build directories

        SetRootSourceDir(ATOMIC_ROOT_SOURCE_DIR);
        SetRootBuildDir(ATOMIC_ROOT_BUILD_DIR, true);

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

#endif

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
    resourceCoreDataDir_ = rootSourceDir_ + "Data/AtomicPlayer/Resources/CoreData";
    resourcePlayerDataDir_ = rootSourceDir_ + "Data/AtomicPlayer/Resources/PlayerData";
    resourceEditorDataDir_ = rootSourceDir_ + "Data/AtomicEditor/Resources/EditorData";
}

void ToolEnvironment::SetRootBuildDir(const String& buildDir, bool setBinaryPaths)
{
    rootBuildDir_ = AddTrailingSlash(buildDir);

    if (setBinaryPaths)
    {
#ifdef ATOMIC_PLATFORM_OSX
        playerBinary_ = rootBuildDir_ + "Source/AtomicPlayer/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";
        editorBinary_ = rootBuildDir_ + "Source/AtomicEditor/AtomicEditor.app/Contents/MacOS/AtomicEditor";
#endif
    }

}

}
