
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class ProjectBuildSettings : public Object
{
    OBJECT(ProjectBuildSettings);

public:
    /// Construct.
    ProjectBuildSettings(Context* context);
    /// Destruct.
    virtual ~ProjectBuildSettings();

    bool Load(const String& path);
    void Save(const String& path);

};

}
