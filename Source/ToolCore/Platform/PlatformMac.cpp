//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include "../Project/Project.h"
#include "../Build/BuildMac.h"
#include "PlatformMac.h"

namespace ToolCore
{

PlatformMac::PlatformMac(Context* context) : Platform(context)
{

}

PlatformMac::~PlatformMac()
{

}

BuildBase* PlatformMac::NewBuild(Project *project)
{
    return new BuildMac(context_, project);
}

}
