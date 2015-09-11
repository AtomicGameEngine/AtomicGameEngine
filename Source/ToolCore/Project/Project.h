//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/Object.h>
#include <Atomic/IO/FileSystem.h>

using namespace Atomic;

#include "../Platform/Platform.h"

namespace ToolCore
{

class ProjectUserPrefs;
class ProjectBuildSettings;

class Project : public Object
{
    friend class ProjectFile;

    OBJECT(Project);

public:
    /// Construct.
    Project(Context* context);
    /// Destruct.
    virtual ~Project();

    bool Load(const String& fullpath);
    void Save(const String& fullpath = "");

    /// Paths
    const String& GetResourcePath() { return resourcePath_; }
    void SetResourcePath(const String& resourcePath)
    {
        resourcePath_ = AddTrailingSlash(resourcePath);
        componentsPath_ = resourcePath_ + "Components";
        scriptsPath_ = resourcePath_ + "Scripts";
        modulesPath_ = resourcePath_ + "Modules";
    }

    const String& GetComponentsPath() { return componentsPath_; }
    const String& GetScriptsPath() { return scriptsPath_; }
    const String& GetModulesPath() { return modulesPath_; }

    bool IsComponentsDirOrFile(const String& fullPath);
    bool IsScriptsDirOrFile(const String& fullPath);
    bool IsModulesDirOrFile(const String& fullPath);

    void AddPlatform(PlatformID platformID);
    bool ContainsPlatform(PlatformID platformID);
    void RemovePlatform(PlatformID platformID);

    bool IsDirty() { return dirty_; }
    void SetDirty() { if (!loading_) dirty_ = true; }

    ProjectBuildSettings* GetBuildSettings() { return buildSettings_; }
    ProjectUserPrefs* GetUserPrefs() { return userPrefs_; }

    const String& GetProjectPath() const { return projectPath_; }
    const String& GetProjectFilePath() { return projectFilePath_; }
    String GetUserPrefsFullPath();
    String GetBuildSettingsFullPath();

    const String& GetVersion() { return version_; }
    void SetVersion(const String& version) { version_ = version; }

    void SaveBuildSettings();
    bool LoadBuildSettings();

    void SaveUserPrefs();
    bool LoadUserPrefs();

private:


    String version_;

    String projectPath_;
    String projectFilePath_;
    String resourcePath_;

    String componentsPath_;
    String scriptsPath_;
    String modulesPath_;

    bool loading_;
    bool dirty_;

    SharedPtr<ProjectUserPrefs> userPrefs_;
    SharedPtr<ProjectBuildSettings> buildSettings_;

    List<PlatformID> platforms_;

};

}
