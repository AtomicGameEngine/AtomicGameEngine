//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "BuildBase.h"

namespace ToolCore
{

class Project;

class BuildMac : public BuildBase
{
    OBJECT(BuildMac);

public:

    BuildMac(Context* context, Project* project);
    virtual ~BuildMac();

    String GetBuildSubfolder() { return "Mac-Build"; }

    void Build(const String& buildPath);

protected:

    void Initialize();


};

}
