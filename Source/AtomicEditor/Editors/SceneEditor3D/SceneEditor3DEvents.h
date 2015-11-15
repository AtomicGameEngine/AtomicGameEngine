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

EVENT(E_SCENENODESELECTED, SceneNodeSelected)
{
    PARAM(P_SCENE, Scene);             // Scene
    PARAM(P_NODE, Node);               // Node
    PARAM(P_SELECTED, Selected);       // bool
}

EVENT(E_SCENEEDITBEGIN, SceneEditBegin)
{
    PARAM(P_SCENE, Scene);             // Scene

}

EVENT(E_SCENEEDITEND, SceneEditEnd)
{
    PARAM(P_SCENE, Scene);             // Scene
}

EVENT(E_SCENEEDITSTATECHANGE, SceneEditStateChange)
{
    PARAM(P_SERIALIZABLE, Serializable);     // Serializable
}

/// A child node has been added to a parent node.
EVENT(E_SCENEEDITNODEADDED, SceneEditNodeAdded)
{
    PARAM(P_SCENE, Scene);                  // Scene pointer
    PARAM(P_PARENT, Parent);                // Node pointer
    PARAM(P_NODE, Node);                    // Node pointer
}

/// A child node is about to be removed from a parent node.
EVENT(E_SCENEEDITNODEREMOVED, SceneEditNodeRemoved)
{
    PARAM(P_SCENE, Scene);                  // Scene pointer
    PARAM(P_PARENT, Parent);                // Node pointer
    PARAM(P_NODE, Node);                    // Node pointer
}


EVENT(E_SCENEEDITADDREMOVENODES, SceneEditAddRemoveNodes)
{
    PARAM(P_END, End);       // bool
}

EVENT(E_SCENEEDITSCENEMODIFIED, SceneEditSceneModified)
{

}

}
