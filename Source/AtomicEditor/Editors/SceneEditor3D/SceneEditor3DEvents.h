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

EVENT(E_HISTORYNODEADDED, HistoryNodeAdded)
{
    PARAM(P_SCENE, Scene);            // Scene
    PARAM(P_NODE, Node);            // Node
}

EVENT(E_HISTORYNODEREMOVED, HistoryNodeRemoved)
{
    PARAM(P_SCENE, Scene);            // Scene
    PARAM(P_NODE, Node);             // Node
}

EVENT(E_HISTORYNODECHANGED, HistoryNodeChanged)
{
    PARAM(P_SCENE, Scene);            // Scene
    PARAM(P_NODE, Node);              // Node
}

EVENT(E_HISTORYCOMPONENTCHANGED, HistoryComponentChanged)
{
    PARAM(P_SCENE, Scene);            // Scene
    PARAM(P_COMPONENT, Component);    // Component
}

EVENT(E_HISTORYNODECHANGEDUNDOREDO, HistoryNodeChangedUndoRedo)
{
    PARAM(P_SCENE, Scene);            // Scene
    PARAM(P_NODE, Node);    // Node
}

EVENT(E_HISTORYCOMPONENTCHANGEDUNDOREDO, HistoryComponentChangedUndoRedo)
{
    PARAM(P_SCENE, Scene);            // Scene
    PARAM(P_COMPONENT, Component);    // Component
}



}
