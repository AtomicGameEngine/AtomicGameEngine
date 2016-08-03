//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Atomic/Core/Object.h>

namespace AtomicEditor
{

/// Variable timestep scene update.
ATOMIC_EVENT(E_GIZMOEDITMODECHANGED, GizmoEditModeChanged)
{
    ATOMIC_PARAM(P_MODE, Mode);            // int
}

ATOMIC_EVENT(E_GIZMOAXISMODECHANGED, GizmoAxisModeChanged)
{
    ATOMIC_PARAM(P_MODE, Mode);            // int
}

ATOMIC_EVENT(E_GIZMOMOVED, GizmoMoved)
{

}

ATOMIC_EVENT(E_SCENENODESELECTED, SceneNodeSelected)
{
    ATOMIC_PARAM(P_SCENE, Scene);             // Scene
    ATOMIC_PARAM(P_NODE, Node);               // Node
    ATOMIC_PARAM(P_SELECTED, Selected);       // bool
    ATOMIC_PARAM(P_QUIET, Quiet);             // bool (don't record edit event)
}

ATOMIC_EVENT(E_SCENEEDITBEGIN, SceneEditBegin)
{
    ATOMIC_PARAM(P_SCENE, Scene);             // Scene

}

ATOMIC_EVENT(E_SCENEEDITNODEREPARENT, SceneEditNodeReparent)
{
    ATOMIC_PARAM(P_NODE, Node);             // Node
    ATOMIC_PARAM(P_ADDED, Added);             // Boolean
}


ATOMIC_EVENT(E_SCENEEDITEND, SceneEditEnd)
{
    ATOMIC_PARAM(P_SCENE, Scene);             // Scene
}

ATOMIC_EVENT(E_SCENEEDITSTATECHANGESBEGIN, SceneEditStateChangesBegin)
{

}

ATOMIC_EVENT(E_SCENEEDITSTATECHANGE, SceneEditStateChange)
{
    ATOMIC_PARAM(P_SERIALIZABLE, Serializable);     // Serializable
}

ATOMIC_EVENT(E_SCENEEDITSTATECHANGESEND, SceneEditStateChangesEnd)
{

}

/// A child node has been added to a parent node.
ATOMIC_EVENT(E_SCENEEDITNODECREATED, SceneEditNodeCreated)
{
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}


/// A child node has been added to a parent node.
ATOMIC_EVENT(E_SCENEEDITNODEADDED, SceneEditNodeAdded)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_PARENT, Parent);                // Node pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

/// A child node is about to be removed from a parent node.
ATOMIC_EVENT(E_SCENEEDITNODEREMOVED, SceneEditNodeRemoved)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_PARENT, Parent);                // Node pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}


/// A child node has been added to a parent node.
ATOMIC_EVENT(E_SCENEEDITCOMPONENTADDEDREMOVED, SceneEditComponentAddedRemoved)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
    ATOMIC_PARAM(P_COMPONENT, Component);          // Component pointer
    ATOMIC_PARAM(P_REMOVED, Removed);          // bool

}

ATOMIC_EVENT(E_SCENEEDITPREFABSAVE, SceneEditPrefabSave)
{
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

ATOMIC_EVENT(E_SCENEEDITPREFABREVERT, SceneEditPrefabRevert)
{
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

ATOMIC_EVENT(E_SCENEEDITPREFABCOPY, SceneEditPrefabCopy)
{
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

ATOMIC_EVENT(E_SCENEEDITPREFABPASTE, SceneEditPrefabPaste)
{
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

ATOMIC_EVENT(E_SCENEEDITPREFABBREAK, SceneEditPrefabBreak)
{
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

ATOMIC_EVENT(E_SCENEEDITCOMPONENTCOPY, SceneEditComponentCopy)
{
    ATOMIC_PARAM(P_COMPONENT, Component);          // Component pointer
}

ATOMIC_EVENT(E_SCENEEDITCOMPONENTPASTE, SceneEditComponentPaste)
{
    ATOMIC_PARAM(P_COMPONENT, Component);          // Component pointer
    ATOMIC_PARAM(P_END, End);                       // bool
}

ATOMIC_EVENT(E_SCENEEDITADDREMOVENODES, SceneEditAddRemoveNodes)
{
    ATOMIC_PARAM(P_END, End);       // bool
}

ATOMIC_EVENT(E_SCENEEDITSCENEMODIFIED, SceneEditSceneModified)
{

}

ATOMIC_EVENT(E_CUBEMAPRENDERBEGIN, CubemapRenderBegin)
{

}

ATOMIC_EVENT(E_CUBEMAPRENDEREND, CubemapRenderEnd)
{

}

}
