
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class Project;

static const int PROJECTFILE_VERSION = 1;

class ProjectFile : public Object
{
    OBJECT(ProjectFile);

public:
    /// Construct.
    ProjectFile(Context* context);
    /// Destruct.
    virtual ~ProjectFile();

    void Save(Project* project);
    bool Load(Project* project);

    void WriteNewProject(const String& fullpath);

private:

    SharedPtr<Project> project_;

};

}
