//
// Copyright (c) 2008-2016 the Urho3D project.
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

#include "../Core/Object.h"

namespace Atomic
{

/// Variable timestep scene update.
ATOMIC_EVENT(E_SCENEUPDATE, SceneUpdate)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Scene subsystem update.
ATOMIC_EVENT(E_SCENESUBSYSTEMUPDATE, SceneSubsystemUpdate)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Scene transform smoothing update.
ATOMIC_EVENT(E_UPDATESMOOTHING, UpdateSmoothing)
{
    ATOMIC_PARAM(P_CONSTANT, Constant);            // float
    ATOMIC_PARAM(P_SQUAREDSNAPTHRESHOLD, SquaredSnapThreshold);  // float
}

/// Scene drawable update finished. Custom animation (eg. IK) can be done at this point.
ATOMIC_EVENT(E_SCENEDRAWABLEUPDATEFINISHED, SceneDrawableUpdateFinished)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// SmoothedTransform target position changed.
ATOMIC_EVENT(E_TARGETPOSITION, TargetPositionChanged)
{
}

/// SmoothedTransform target position changed.
ATOMIC_EVENT(E_TARGETROTATION, TargetRotationChanged)
{
}

/// Scene attribute animation update.
ATOMIC_EVENT(E_ATTRIBUTEANIMATIONUPDATE, AttributeAnimationUpdate)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Attribute animation added to object animation.
ATOMIC_EVENT(E_ATTRIBUTEANIMATIONADDED, AttributeAnimationAdded)
{
    ATOMIC_PARAM(P_OBJECTANIMATION, ObjectAnimation);               // Object animation pointer
    ATOMIC_PARAM(P_ATTRIBUTEANIMATIONNAME, AttributeAnimationName); // String
}

/// Attribute animation removed from object animation.
ATOMIC_EVENT(E_ATTRIBUTEANIMATIONREMOVED, AttributeAnimationRemoved)
{
    ATOMIC_PARAM(P_OBJECTANIMATION, ObjectAnimation);               // Object animation pointer
    ATOMIC_PARAM(P_ATTRIBUTEANIMATIONNAME, AttributeAnimationName); // String
}

/// Variable timestep scene post-update.
ATOMIC_EVENT(E_SCENEPOSTUPDATE, ScenePostUpdate)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Asynchronous scene loading progress.
ATOMIC_EVENT(E_ASYNCLOADPROGRESS, AsyncLoadProgress)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_PROGRESS, Progress);            // float
    ATOMIC_PARAM(P_LOADEDNODES, LoadedNodes);      // int
    ATOMIC_PARAM(P_TOTALNODES, TotalNodes);        // int
    ATOMIC_PARAM(P_LOADEDRESOURCES, LoadedResources); // int
    ATOMIC_PARAM(P_TOTALRESOURCES, TotalResources);   // int
};

/// Asynchronous scene loading finished.
ATOMIC_EVENT(E_ASYNCLOADFINISHED, AsyncLoadFinished)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
};

/// A child node has been added to a parent node.
ATOMIC_EVENT(E_NODEADDED, NodeAdded)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_PARENT, Parent);                // Node pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

/// A child node is about to be removed from a parent node.
ATOMIC_EVENT(E_NODEREMOVED, NodeRemoved)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_PARENT, Parent);                // Node pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

/// A component has been created to a node.
ATOMIC_EVENT(E_COMPONENTADDED, ComponentAdded)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
    ATOMIC_PARAM(P_COMPONENT, Component);          // Component pointer
}

/// A component is about to be removed from a node.
ATOMIC_EVENT(E_COMPONENTREMOVED, ComponentRemoved)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
    ATOMIC_PARAM(P_COMPONENT, Component);          // Component pointer
}

/// A node's name has changed.
ATOMIC_EVENT(E_NODENAMECHANGED, NodeNameChanged)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

/// A node's enabled state has changed.
ATOMIC_EVENT(E_NODEENABLEDCHANGED, NodeEnabledChanged)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
}

/// A node's tag has been added.
ATOMIC_EVENT(E_NODETAGADDED, NodeTagAdded)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
    ATOMIC_PARAM(P_TAG, Tag);                      // String tag
}

/// A node's tag has been removed.
ATOMIC_EVENT(E_NODETAGREMOVED, NodeTagRemoved)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
    ATOMIC_PARAM(P_TAG, Tag);                      // String tag
}

/// A component's enabled state has changed.
ATOMIC_EVENT(E_COMPONENTENABLEDCHANGED, ComponentEnabledChanged)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
    ATOMIC_PARAM(P_COMPONENT, Component);          // Component pointer
}

/// A serializable's temporary state has changed.
ATOMIC_EVENT(E_TEMPORARYCHANGED, TemporaryChanged)
{
    ATOMIC_PARAM(P_SERIALIZABLE, Serializable);    // Serializable pointer
}

/// A node (and its children and components) has been cloned.
ATOMIC_EVENT(E_NODECLONED, NodeCloned)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_NODE, Node);                    // Node pointer
    ATOMIC_PARAM(P_CLONENODE, CloneNode);          // Node pointer
}

/// A component has been cloned.
ATOMIC_EVENT(E_COMPONENTCLONED, ComponentCloned)
{
    ATOMIC_PARAM(P_SCENE, Scene);                  // Scene pointer
    ATOMIC_PARAM(P_COMPONENT, Component);          // Component pointer
    ATOMIC_PARAM(P_CLONECOMPONENT, CloneComponent); // Component pointer
}

/// A network attribute update from the server has been intercepted.
ATOMIC_EVENT(E_INTERCEPTNETWORKUPDATE, InterceptNetworkUpdate)
{
    ATOMIC_PARAM(P_SERIALIZABLE, Serializable);    // Serializable pointer
    ATOMIC_PARAM(P_TIMESTAMP, TimeStamp);          // unsigned (0-255)
    ATOMIC_PARAM(P_INDEX, Index);                  // unsigned
    ATOMIC_PARAM(P_NAME, Name);                    // String
    ATOMIC_PARAM(P_VALUE, Value);                  // Variant
}

}
