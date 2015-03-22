#include <Atomic/Core/Object.h>
#include <Atomic/IO/FileSystem.h>

using namespace Atomic;

namespace ToolCore
{

class Project : public Object
{
    OBJECT(Project);

public:
    /// Construct.
    Project(Context* context);
    /// Destruct.
    virtual ~Project();

    const String GetResourcePath() { return resourcePath_; }
    void SetResourcePath(const String& resourcePath)
    {
        resourcePath_ = AddTrailingSlash(resourcePath);
        componentsPath_ = resourcePath_ + "Components";
        scriptsPath_ = resourcePath_ + "Scripts";
        modulesPath_ = resourcePath_ + "Modules";
    }
    void Load(const String& fullpath);
    void Save(const String& fullpath = "");

    const String& GetComponentsPath() { return componentsPath_; }
    const String& GetScriptsPath() { return scriptsPath_; }
    const String& GetModulesPath() { return modulesPath_; }

    const String& GetProjectFilePath() { return projectFilePath_; }

    bool IsComponentsDirOrFile(const String& fullPath);
    bool IsScriptsDirOrFile(const String& fullPath);
    bool IsModulesDirOrFile(const String& fullPath);

    const String& GetLastBuildPath() { return lastBuildPath_; }
    void SetLastBuildPath(const String& path) { lastBuildPath_ = path; }

private:

    void LoadUserPrefs(const String& fullpath);
    void SaveUserPrefs(const String& fullpath);
    String GetUserPrefsFullPath(const String& projectPath);

    String projectFilePath_;
    String resourcePath_;

    String componentsPath_;
    String scriptsPath_;
    String modulesPath_;

    String lastBuildPath_;

};

}
