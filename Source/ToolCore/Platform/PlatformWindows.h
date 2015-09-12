//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "Platform.h"

namespace ToolCore
{

class PlatformWindows : public Platform
{
    OBJECT(PlatformWindows);

public:

    PlatformWindows(Context* context);
    virtual ~PlatformWindows();

    String GetName() { return "WINDOWS"; }
    PlatformID GetPlatformID() { return PLATFORMID_WINDOWS; }

    BuildBase* NewBuild(Project* project);

    virtual bool GetLicense() { return true; }

};

}
