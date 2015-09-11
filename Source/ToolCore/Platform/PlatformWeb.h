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

class PlatformWeb : public Platform
{
    OBJECT(PlatformWeb);

public:

    PlatformWeb(Context* context);
    virtual ~PlatformWeb();

    String GetName() { return "WEB"; }
    PlatformID GetPlatformID() { return PLATFORMID_WEB; }

    BuildBase* NewBuild(Project *project);

    virtual bool GetLicense() { return true; }

};

}
