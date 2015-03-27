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
    const String GetResourcePath() { return resourcePath_; }
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

    const String& GetProjectFilePath() { return projectFilePath_; }

    bool IsComponentsDirOrFile(const String& fullPath);
    bool IsScriptsDirOrFile(const String& fullPath);
    bool IsModulesDirOrFile(const String& fullPath);

    void SaveBuildSettings(const String& path);
    bool LoadBuildSettings(const String& path);

    void AddPlatform(PlatformID platformID);
    bool ContainsPlatform(PlatformID platformID);
    void RemovePlatform(PlatformID platformID);

    bool IsDirty() { return dirty_; }

    ProjectBuildSettings* GetBuildSettings();

    String GetUserPrefsFullPath();
    String GetBuildSettingsFullPath();

private:

    void LoadUserPrefs(const String& fullpath);
    void SaveUserPrefs(const String& fullpath);

    String projectFilePath_;
    String resourcePath_;

    String componentsPath_;
    String scriptsPath_;
    String modulesPath_;

    bool dirty_;

    SharedPtr<ProjectUserPrefs> userPrefs_;
    SharedPtr<ProjectBuildSettings> buildSettings_;

    List<PlatformID> platforms_;

};

}
