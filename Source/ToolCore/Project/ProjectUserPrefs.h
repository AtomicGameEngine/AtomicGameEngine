
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

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

    const String& GetLastBuildPath() { return lastBuildPath_; }
    void SetLastBuildPath(const String& path) { lastBuildPath_ = path; }

private:

    String lastBuildPath_;

};

}
