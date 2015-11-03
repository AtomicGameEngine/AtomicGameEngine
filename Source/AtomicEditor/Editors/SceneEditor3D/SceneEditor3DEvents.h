//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

namespace AtomicEditor
{

/// Variable timestep scene update.
EVENT(E_GIZMOEDITMODECHANGED, GizmoEditModeChanged)
{
    PARAM(P_MODE, MODE);            // int
}

EVENT(E_GIZMOAXISMODECHANGED, GizmoAxisModeChanged)
{
    PARAM(P_MODE, MODE);            // int
}

EVENT(E_GIZMOMOVED, GizmoMoved)
{

}

}
