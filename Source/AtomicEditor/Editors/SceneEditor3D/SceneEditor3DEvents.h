
#pragma once

#include <Atomic/Core/Object.h>

namespace AtomicEditor
{

/// Variable timestep scene update.
EVENT(E_GIZMOEDITMODECHANGED, GizmoEditModeChanged)
{
    PARAM(P_MODE, MODE);            // int
}

EVENT(E_GIZMOMOVED, GizmoMoved)
{

}

}
