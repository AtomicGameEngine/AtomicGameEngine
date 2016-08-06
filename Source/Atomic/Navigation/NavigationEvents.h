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

/// Complete rebuild of navigation mesh.
ATOMIC_EVENT(E_NAVIGATION_MESH_REBUILT, NavigationMeshRebuilt)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_MESH, Mesh); // NavigationMesh pointer
}

/// Partial bounding box rebuild of navigation mesh.
ATOMIC_EVENT(E_NAVIGATION_AREA_REBUILT, NavigationAreaRebuilt)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_MESH, Mesh); // NavigationMesh pointer
    ATOMIC_PARAM(P_BOUNDSMIN, BoundsMin); // Vector3
    ATOMIC_PARAM(P_BOUNDSMAX, BoundsMax); // Vector3
}

/// Crowd agent formation.
ATOMIC_EVENT(E_CROWD_AGENT_FORMATION, CrowdAgentFormation)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_INDEX, Index); // unsigned
    ATOMIC_PARAM(P_SIZE, Size); // unsigned
    ATOMIC_PARAM(P_POSITION, Position); // Vector3 [in/out]
}

/// Crowd agent formation specific to a node.
ATOMIC_EVENT(E_CROWD_AGENT_NODE_FORMATION, CrowdAgentNodeFormation)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_INDEX, Index); // unsigned
    ATOMIC_PARAM(P_SIZE, Size); // unsigned
    ATOMIC_PARAM(P_POSITION, Position); // Vector3 [in/out]
}

/// Crowd agent has been repositioned.
ATOMIC_EVENT(E_CROWD_AGENT_REPOSITION, CrowdAgentReposition)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_VELOCITY, Velocity); // Vector3
    ATOMIC_PARAM(P_ARRIVED, Arrived); // bool
    ATOMIC_PARAM(P_TIMESTEP, TimeStep); // float
}

/// Crowd agent has been repositioned, specific to a node
ATOMIC_EVENT(E_CROWD_AGENT_NODE_REPOSITION, CrowdAgentNodeReposition)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_VELOCITY, Velocity); // Vector3
    ATOMIC_PARAM(P_ARRIVED, Arrived); // bool
    ATOMIC_PARAM(P_TIMESTEP, TimeStep); // float
}

/// Crowd agent's internal state has become invalidated. This is a special case of CrowdAgentStateChanged event.
ATOMIC_EVENT(E_CROWD_AGENT_FAILURE, CrowdAgentFailure)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_VELOCITY, Velocity); // Vector3
    ATOMIC_PARAM(P_CROWD_AGENT_STATE, CrowdAgentState); // int
    ATOMIC_PARAM(P_CROWD_TARGET_STATE, CrowdTargetState); // int
}

/// Crowd agent's internal state has become invalidated. This is a special case of CrowdAgentStateChanged event.
ATOMIC_EVENT(E_CROWD_AGENT_NODE_FAILURE, CrowdAgentNodeFailure)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_VELOCITY, Velocity); // Vector3
    ATOMIC_PARAM(P_CROWD_AGENT_STATE, CrowdAgentState); // int
    ATOMIC_PARAM(P_CROWD_TARGET_STATE, CrowdTargetState); // int
}

/// Crowd agent's state has been changed.
ATOMIC_EVENT(E_CROWD_AGENT_STATE_CHANGED, CrowdAgentStateChanged)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_VELOCITY, Velocity); // Vector3
    ATOMIC_PARAM(P_CROWD_AGENT_STATE, CrowdAgentState); // int
    ATOMIC_PARAM(P_CROWD_TARGET_STATE, CrowdTargetState); // int
}

/// Crowd agent's state has been changed.
ATOMIC_EVENT(E_CROWD_AGENT_NODE_STATE_CHANGED, CrowdAgentNodeStateChanged)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_CROWD_AGENT, CrowdAgent); // CrowdAgent pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_VELOCITY, Velocity); // Vector3
    ATOMIC_PARAM(P_CROWD_AGENT_STATE, CrowdAgentState); // int
    ATOMIC_PARAM(P_CROWD_TARGET_STATE, CrowdTargetState); // int
}

/// Addition of obstacle to dynamic navigation mesh.
ATOMIC_EVENT(E_NAVIGATION_OBSTACLE_ADDED, NavigationObstacleAdded)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_OBSTACLE, Obstacle); // Obstacle pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_RADIUS, Radius); // float
    ATOMIC_PARAM(P_HEIGHT, Height); // float
}

/// Removal of obstacle from dynamic navigation mesh.
ATOMIC_EVENT(E_NAVIGATION_OBSTACLE_REMOVED, NavigationObstacleRemoved)
{
    ATOMIC_PARAM(P_NODE, Node); // Node pointer
    ATOMIC_PARAM(P_OBSTACLE, Obstacle); // Obstacle pointer
    ATOMIC_PARAM(P_POSITION, Position); // Vector3
    ATOMIC_PARAM(P_RADIUS, Radius); // float
    ATOMIC_PARAM(P_HEIGHT, Height); // float
}

}
