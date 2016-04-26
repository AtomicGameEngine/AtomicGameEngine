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
EVENT(E_GIZMOEDITMODECHANGED, GizmoEditModeChanged)
{
    PARAM(P_MODE, Mode);            // int
}

EVENT(E_GIZMOAXISMODECHANGED, GizmoAxisModeChanged)
{
    PARAM(P_MODE, Mode);            // int
}

EVENT(E_GIZMOMOVED, GizmoMoved)
{

}

EVENT(E_SCENENODESELECTED, SceneNodeSelected)
{
    PARAM(P_SCENE, Scene);             // Scene
    PARAM(P_NODE, Node);               // Node
    PARAM(P_SELECTED, Selected);       // bool
    PARAM(P_QUIET, Quiet);             // bool (don't record edit event)
}

EVENT(E_SCENEEDITBEGIN, SceneEditBegin)
{
    PARAM(P_SCENE, Scene);             // Scene

}

EVENT(E_SCENEEDITNODEREPARENT, SceneEditNodeReparent)
{
    PARAM(P_NODE, Node);             // Node
    PARAM(P_ADDED, Added);             // Boolean
}


EVENT(E_SCENEEDITEND, SceneEditEnd)
{
    PARAM(P_SCENE, Scene);             // Scene
}

EVENT(E_SCENEEDITSTATECHANGESBEGIN, SceneEditStateChangesBegin)
{

}

EVENT(E_SCENEEDITSTATECHANGE, SceneEditStateChange)
{
    PARAM(P_SERIALIZABLE, Serializable);     // Serializable
}

EVENT(E_SCENEEDITSTATECHANGESEND, SceneEditStateChangesEnd)
{

}

/// A child node has been added to a parent node.
EVENT(E_SCENEEDITNODECREATED, SceneEditNodeCreated)
{
    PARAM(P_NODE, Node);                    // Node pointer
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


/// A child node has been added to a parent node.
EVENT(E_SCENEEDITCOMPONENTADDEDREMOVED, SceneEditComponentAddedRemoved)
{
    PARAM(P_SCENE, Scene);                  // Scene pointer
    PARAM(P_NODE, Node);                    // Node pointer
    PARAM(P_COMPONENT, Component);          // Component pointer
    PARAM(P_REMOVED, Removed);          // bool

}

EVENT(E_SCENEEDITPREFABSAVE, SceneEditPrefabSave)
{
    PARAM(P_NODE, Node);                    // Node pointer
}

EVENT(E_SCENEEDITPREFABREVERT, SceneEditPrefabRevert)
{
    PARAM(P_NODE, Node);                    // Node pointer
}


EVENT(E_SCENEEDITPREFABBREAK, SceneEditPrefabBreak)
{
    PARAM(P_NODE, Node);                    // Node pointer
}

EVENT(E_SCENEEDITADDREMOVENODES, SceneEditAddRemoveNodes)
{
    PARAM(P_END, End);       // bool
}

EVENT(E_SCENEEDITSCENEMODIFIED, SceneEditSceneModified)
{

}

EVENT(E_CUBEMAPRENDERBEGIN, CubemapRenderBegin)
{

}

EVENT(E_CUBEMAPRENDEREND, CubemapRenderEnd)
{

}

}
