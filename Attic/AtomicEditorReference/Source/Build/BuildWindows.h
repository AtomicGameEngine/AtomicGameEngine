// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "BuildBase.h"

namespace AtomicEditor
{

class BuildWindows : public BuildBase
{
    OBJECT(BuildWindows);

public:

    BuildWindows(Context* context);
    virtual ~BuildWindows();

    void Build(const String& buildPath);

protected:

    void Initialize();


};

}
