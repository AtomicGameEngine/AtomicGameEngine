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
    PARAM(P_MODE, Mode);            // int
}

EVENT(E_GIZMOAXISMODECHANGED, GizmoAxisModeChanged)
{
    PARAM(P_MODE, Mode);            // int
    PARAM(P_TOGGLE, Toggle);            // bool

}

EVENT(E_GIZMOMOVED, GizmoMoved)
{

}

EVENT(E_SCENEEDITSCENEMODIFIED, SceneEditSceneModified)
{

}

EVENT(E_SCENEEDITNODEADDEDREMOVED, SceneEditNodeAddedRemoved)
{
    PARAM(P_SCENE, Scene);             // Scene
    PARAM(P_NODE, Node);               // Node
    PARAM(P_ADDED, Added);             // bool
}

EVENT(E_SCENEEDITCOMPONENTADDEDREMOVED, SceneEditComponentAddedRemoved)
{
    PARAM(P_SCENE, Scene);             // Scene
    PARAM(P_COMPONENT, Component);     // Component
    PARAM(P_ADDED, Added);             // bool
}

EVENT(E_SCENEEDITSERIALIZABLE, SceneEditSerializable)
{
    PARAM(P_SERIALIZABLE, Serializable);     // Serializable
    PARAM(P_OPERATION, Operation);           // int (0: begin, 1: change, 2: end)
}

EVENT(E_SCENEEDITSERIALIZABLEUNDOREDO, SceneEditSerializableUndoRedo)
{
    PARAM(P_SERIALIZABLE, Serializable);     // Serializable
    PARAM(P_STATE, State);                   // State (VectorBuffer);
    PARAM(P_UNDO, Undo);                     // bool (true: undo, false: redo)
}


}
