//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <rapidjson/document.h>
#include <rapidjson/filestream.h>
#include <rapidjson/prettywriter.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include <Atomic/Resource/JSONFile.h>

#include "../ToolSystem.h"
#include "../ToolEvents.h"
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
    loading_(false),
    dirty_(false)
{
    version_ = "1.0.0";
    userPrefs_ = new ProjectUserPrefs(context_);
    buildSettings_ = new ProjectBuildSettings(context_);
}

Project::~Project()
{

}

void Project::SaveUserPrefs()
{
    String path = GetProjectPath() + "UserPrefs.json";

    userPrefs_->Save(path);

}

bool Project::LoadUserPrefs()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    String path = GetProjectPath() + "UserPrefs.json";

    userPrefs_->Load(path);

    // If we're in CLI mode, the Build folder is always relative to project
    if (tsystem->IsCLI())
    {
        String path = GetPath(projectFilePath_) + "Build";
        userPrefs_->SetLastBuildPath(path);
    }

    return true;
}

void Project::SaveBuildSettings()
{
    buildSettings_->Save(GetProjectPath() + "BuildSettings.json");
}

bool Project::LoadBuildSettings()
{
    buildSettings_->Load(GetProjectPath() + "BuildSettings.json");
    return true;
}

void Project::AddPlatform(PlatformID platformID)
{
    if (ContainsPlatform(platformID))
        return;

    SetDirty();

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
    loading_ = true;

    if (fullpath.Contains(".atomic")) {

        projectPath_ = AddTrailingSlash(GetPath(fullpath));
        projectFilePath_ = fullpath;

    }
    else
    {
        projectPath_ = AddTrailingSlash(fullpath);
        projectFilePath_ = projectPath_ + GetFileName(RemoveTrailingSlash(projectPath_)) + ".atomic";

    }


    SharedPtr<ProjectFile> pfile(new ProjectFile(context_));
    bool result = pfile->Load(this);

    loading_ = false;

    LoadBuildSettings();
    LoadUserPrefs();

    if ( true /*result*/) {
        VariantMap data;
        data[ProjectLoaded::P_PROJECTPATH] = projectFilePath_;
        SendEvent(E_PROJECTLOADED, data);
    }

    return result;
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
