// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

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

    void Build(const String& buildPath);

protected:

    void Initialize();


};

}
