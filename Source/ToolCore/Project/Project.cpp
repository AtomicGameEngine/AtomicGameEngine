
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <rapidjson/document.h>
#include <rapidjson/filestream.h>
#include <rapidjson/prettywriter.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include <Atomic/Resource/JSONFile.h>

#include "../Platform/Platform.h"

#include "ProjectFile.h"
#include "ProjectBuildSettings.h"
#include "ProjectUserPrefs.h"
#include "Project.h"

using namespace rapidjson;

namespace ToolCore
{

Project::Project(Context* context) :
    Object(context),
    dirty_(false)
{
    userPrefs_ = new ProjectUserPrefs(context_);
    buildSettings_ = new ProjectBuildSettings(context_);
}

Project::~Project()
{

}

void Project::LoadUserPrefs(const String& fullpath)
{
    rapidjson::Document document;

    File jsonFile(context_, fullpath);

    if (!jsonFile.IsOpen())
        return;

    String json;
    jsonFile.ReadText(json);

    if (!json.Length())
        return;

    if (document.Parse<0>(json.CString()).HasParseError())
    {
        LOGERRORF("Could not parse Project JSON data from %s", fullpath.CString());
        return;
    }

    const Value::Member* current_platform = document.FindMember("current_platform");

    /*
    AEEditorPlatform platform = AE_PLATFORM_UNDEFINED;
    if (current_platform && current_platform->value.IsString())
    {
        String splatform = current_platform->value.GetString();
        if (splatform == "Windows")
            platform = AE_PLATFORM_WINDOWS;
        else if (splatform == "Mac")
            platform = AE_PLATFORM_MAC;
        else if (splatform == "HTML5")
            platform = AE_PLATFORM_HTML5;
        else if (splatform == "iOS")
            platform = AE_PLATFORM_IOS;
        else if (splatform == "Android")
            platform = AE_PLATFORM_ANDROID;
    }
    if (platform == AE_PLATFORM_UNDEFINED)
    {
#ifdef ATOMIC_PLATFORM_OSX
        platform = AE_PLATFORM_MAC;
#else
        platform = AE_PLATFORM_WINDOWS;
#endif
    }
    */

    /*
    const Value::Member* last_build_path = document.FindMember("last_build_path");
    if (last_build_path && last_build_path->value.IsString())
    {
        lastBuildPath_ = last_build_path->value.GetString();
    }
    */

    // probably will want to move this, it will trigger a save (which is guarded with load_)
    /*
    Editor* editor = GetSubsystem<Editor>();
    editor->RequestPlatformChange(platform);
    */

}

void Project::SaveUserPrefs(const String& fullpath)
{

}

void Project::SaveBuildSettings(const String& path)
{

}

bool Project::LoadBuildSettings(const String& path)
{
    return false;
}

void Project::AddPlatform(PlatformID platformID)
{
    if (ContainsPlatform(platformID))
        return;

    dirty_ = true;

    platforms_.Push(platformID);

}

void Project::RemovePlatform(PlatformID platformID)
{
    if (!ContainsPlatform(platformID))
        return;

}

bool Project::ContainsPlatform(PlatformID platformID)
{
    for (List<PlatformID>::ConstIterator i = platforms_.Begin(); i != platforms_.End(); ++i)
    {
        if ((*i) == platformID)
            return true;
    }

    return false;

}

bool Project::Load(const String& fullpath)
{
    projectFilePath_ = fullpath;
    SharedPtr<ProjectFile> pfile(new ProjectFile(context_));
    return pfile->Load(this);
}

String Project::GetBuildSettingsFullPath()
{
    String path = GetPath(projectFilePath_);
    String filename = GetFileName(projectFilePath_);
    String buildSettingsPath = path + filename + ".buildsettings";
    return buildSettingsPath;
}

String Project::GetUserPrefsFullPath()
{
    String path = GetPath(projectFilePath_);
    String filename = GetFileName(projectFilePath_);
    String prefsPath = path + filename + ".userprefs";
    return prefsPath;
}

void Project::Save(const String& fullpath)
{
    SharedPtr<ProjectFile> pfile(new ProjectFile(context_));
    pfile->Save(this);
    dirty_ = false;
}

bool Project::IsComponentsDirOrFile(const String& fullPath)
{
    String pathName;
    String fileName;
    String extension;

    SplitPath(fullPath, pathName, fileName, extension);

    if (extension.Length() && extension != ".js")
        return false;

    if (IsAbsoluteParentPath(componentsPath_, pathName))
        return true;

    return false;

}

bool Project::IsScriptsDirOrFile(const String& fullPath)
{
    String pathName;
    String fileName;
    String extension;

    SplitPath(fullPath, pathName, fileName, extension);

    if (extension.Length() && extension != ".js")
        return false;

    if (IsAbsoluteParentPath(scriptsPath_, pathName))
        return true;

    return false;

}

bool Project::IsModulesDirOrFile(const String& fullPath)
{
    String pathName;
    String fileName;
    String extension;

    SplitPath(fullPath, pathName, fileName, extension);

    if (extension.Length() && extension != ".js")
        return false;

    if (IsAbsoluteParentPath(modulesPath_, pathName))
        return true;

    return false;

}


}
