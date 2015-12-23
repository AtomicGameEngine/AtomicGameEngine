//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

EVENT(E_IPCPLAYEREXITREQUEST, IPCPlayerExitRequest)
{

}

EVENT(E_IPCPLAYERWINDOWCHANGED, IPCPlayerWindowChanged)
{
    PARAM(P_POSX, PosX);
    PARAM(P_POSY, PosY);
    PARAM(P_WIDTH, Width);
    PARAM(P_HEIGHT, Height);
    PARAM(P_MONITOR, Monitor);
}

EVENT(E_PLAYERQUIT, PlayerQuit)
{

}

}
