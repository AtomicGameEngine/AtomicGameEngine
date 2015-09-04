// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "BuildBase.h"

namespace ToolCore
{

class Project;

class BuildIOS : public BuildBase
{
    OBJECT(BuildIOS);

public:

    BuildIOS(Context* context, Project* project);
    virtual ~BuildIOS();

    String GetBuildSubfolder() { return "iOS-Build"; }

    void Build(const String& buildPath);

protected:

    void Initialize();


};

}
