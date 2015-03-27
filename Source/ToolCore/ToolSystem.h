
#pragma once

#include <Atomic/Core/Object.h>

#include "Platform/Platform.h"

using namespace Atomic;

namespace ToolCore
{

class Platform;
class Project;

class ToolSystem : public Object
{
    OBJECT(ToolSystem);

public:

    ToolSystem(Context* context);
    virtual ~ToolSystem();

    bool LoadProject(const String& fullpath);
    Project* GetProject() { return project_; }

    const String& GetDataPath() { return dataPath_; }
    void SetDataPath(const String& path) { dataPath_ = path; }

    // Platforms
    void RegisterPlatform(Platform* platform);
    Platform* GetPlatformByID(PlatformID platform);
    Platform* GetPlatformByName(const String& name);

    void SetCurrentPlatform(PlatformID platform);
    Platform* GetCurrentPlatform();

private:

    /// Full path to data files
    String dataPath_;

    SharedPtr<Platform> currentPlatform_;

    // PlatformID -> platform
    HashMap<unsigned, SharedPtr<Platform> > platforms_;

    SharedPtr<Project> project_;

};

}
