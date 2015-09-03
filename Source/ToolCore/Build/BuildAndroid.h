// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "BuildBase.h"

namespace ToolCore
{

class Project;

class BuildAndroid : public BuildBase
{
    OBJECT(BuildAndroid);

public:

    BuildAndroid(Context* context, Project* project);
    virtual ~BuildAndroid();

    void Build(const String& buildPath);

    String GetBuildSubfolder() { return "Android-Build"; }

protected:

    void Initialize();


};

}
