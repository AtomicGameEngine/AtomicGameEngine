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

class PlatformIOS : public Platform
{
    OBJECT(PlatformIOS);

public:

    PlatformIOS(Context* context);
    virtual ~PlatformIOS();

    String GetName() { return "IOS"; }
    PlatformID GetPlatformID() { return PLATFORMID_IOS; }

    BuildBase* NewBuild(Project* project);

    String ParseProvisionAppIdentifierPrefix(const String& provisionFile);

    bool GetLicense();

};

}
