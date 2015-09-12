//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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
