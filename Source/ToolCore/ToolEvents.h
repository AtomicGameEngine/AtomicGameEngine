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

EVENT(E_PROJECTLOADED, ProjectLoaded)
{
    PARAM(P_PROJECTPATH, ProjectPath);    // string
}

EVENT(E_PROJECTUNLOADED, ProjectUnloaded)
{
}

EVENT(E_PLATFORMCHANGED, PlatformChanged)
{
    PARAM(P_PLATFORM, Platform);    // Platform Ptr
}


}
