
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

#include "../Platform/Platform.h"

namespace ToolCore
{

class ProjectUserPrefs : public Object
{
    OBJECT(ProjectUserPrefs);

public:
    /// Construct.
    ProjectUserPrefs(Context* context);
    /// Destruct.
    virtual ~ProjectUserPrefs();

    // platform used when not specified
    PlatformID GetDefaultPlatform();

    const String& GetLastBuildPath() { return lastBuildPath_; }
    void SetLastBuildPath(const String& path) { lastBuildPath_ = path; }

private:
    PlatformID defaultPlatform_;
    String lastBuildPath_;

};

}
