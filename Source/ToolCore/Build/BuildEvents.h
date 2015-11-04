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

EVENT(E_BUILDCOMPLETE, BuildComplete)
{
    PARAM(P_PLATFORMID, PlatformID); // unsigned (PLATFORMID)
    PARAM(P_BUILDFOLDER, BuildFolder); // String
    PARAM(P_MESSAGE, Message); // String
    PARAM(P_SUCCESS, Success); // Bool
}

EVENT(E_BUILDOUTPUT, BuildOutput)
{
    PARAM(P_TEXT, Text); // string
}

}
