// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

EVENT(E_BUILDCOMPLETE, BuildComplete)
{
    PARAM(P_PLATFORMID, PlatformID); // unsigned (PLATFORMID)
}

EVENT(E_BUILDOUTPUT, BuildOutput)
{
    PARAM(P_TEXT, Text); // string
}

EVENT(E_BUILDFAILED, BuildFailed)
{
    PARAM(P_PLATFORMID, PlatformID); // unsigned (PLATFORMID)
    PARAM(P_MESSAGE, Message); // String
}

}
