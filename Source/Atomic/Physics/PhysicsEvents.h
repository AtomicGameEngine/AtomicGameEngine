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

/// Physics world is about to be stepped.
ATOMIC_EVENT(E_PHYSICSPRESTEP, PhysicsPreStep)
{
    ATOMIC_PARAM(P_WORLD, World);                  // PhysicsWorld pointer
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Physics world has been stepped.
ATOMIC_EVENT(E_PHYSICSPOSTSTEP, PhysicsPostStep)
{
    ATOMIC_PARAM(P_WORLD, World);                  // PhysicsWorld pointer
    ATOMIC_PARAM(P_TIMESTEP, TimeStep);            // float
}

/// Physics collision started. Global event sent by the PhysicsWorld.
ATOMIC_EVENT(E_PHYSICSCOLLISIONSTART, PhysicsCollisionStart)
{
    ATOMIC_PARAM(P_WORLD, World);                  // PhysicsWorld pointer
    ATOMIC_PARAM(P_NODEA, NodeA);                  // Node pointer
    ATOMIC_PARAM(P_NODEB, NodeB);                  // Node pointer
    ATOMIC_PARAM(P_BODYA, BodyA);                  // RigidBody pointer
    ATOMIC_PARAM(P_BODYB, BodyB);                  // RigidBody pointer
    ATOMIC_PARAM(P_TRIGGER, Trigger);              // bool
    ATOMIC_PARAM(P_CONTACTS, Contacts);            // Buffer containing position (Vector3), normal (Vector3), distance (float), impulse (float) for each contact
}

/// Physics collision ongoing. Global event sent by the PhysicsWorld.
ATOMIC_EVENT(E_PHYSICSCOLLISION, PhysicsCollision)
{
    ATOMIC_PARAM(P_WORLD, World);                  // PhysicsWorld pointer
    ATOMIC_PARAM(P_NODEA, NodeA);                  // Node pointer
    ATOMIC_PARAM(P_NODEB, NodeB);                  // Node pointer
    ATOMIC_PARAM(P_BODYA, BodyA);                  // RigidBody pointer
    ATOMIC_PARAM(P_BODYB, BodyB);                  // RigidBody pointer
    ATOMIC_PARAM(P_TRIGGER, Trigger);              // bool
    ATOMIC_PARAM(P_CONTACTS, Contacts);            // Buffer containing position (Vector3), normal (Vector3), distance (float), impulse (float) for each contact
}

/// Physics collision ended. Global event sent by the PhysicsWorld.
ATOMIC_EVENT(E_PHYSICSCOLLISIONEND, PhysicsCollisionEnd)
{
    ATOMIC_PARAM(P_WORLD, World);                  // PhysicsWorld pointer
    ATOMIC_PARAM(P_NODEA, NodeA);                  // Node pointer
    ATOMIC_PARAM(P_NODEB, NodeB);                  // Node pointer
    ATOMIC_PARAM(P_BODYA, BodyA);                  // RigidBody pointer
    ATOMIC_PARAM(P_BODYB, BodyB);                  // RigidBody pointer
    ATOMIC_PARAM(P_TRIGGER, Trigger);              // bool
}

/// Node's physics collision started. Sent by scene nodes participating in a collision.
ATOMIC_EVENT(E_NODECOLLISIONSTART, NodeCollisionStart)
{
    ATOMIC_PARAM(P_BODY, Body);                    // RigidBody pointer
    ATOMIC_PARAM(P_OTHERNODE, OtherNode);          // Node pointer
    ATOMIC_PARAM(P_OTHERBODY, OtherBody);          // RigidBody pointer
    ATOMIC_PARAM(P_TRIGGER, Trigger);              // bool
    ATOMIC_PARAM(P_CONTACTS, Contacts);            // Buffer containing position (Vector3), normal (Vector3), distance (float), impulse (float) for each contact
}

/// Node's physics collision ongoing. Sent by scene nodes participating in a collision.
ATOMIC_EVENT(E_NODECOLLISION, NodeCollision)
{
    ATOMIC_PARAM(P_BODY, Body);                    // RigidBody pointer
    ATOMIC_PARAM(P_OTHERNODE, OtherNode);          // Node pointer
    ATOMIC_PARAM(P_OTHERBODY, OtherBody);          // RigidBody pointer
    ATOMIC_PARAM(P_TRIGGER, Trigger);              // bool
    ATOMIC_PARAM(P_CONTACTS, Contacts);            // Buffer containing position (Vector3), normal (Vector3), distance (float), impulse (float) for each contact
}

/// Node's physics collision ended. Sent by scene nodes participating in a collision.
ATOMIC_EVENT(E_NODECOLLISIONEND, NodeCollisionEnd)
{
    ATOMIC_PARAM(P_BODY, Body);                    // RigidBody pointer
    ATOMIC_PARAM(P_OTHERNODE, OtherNode);          // Node pointer
    ATOMIC_PARAM(P_OTHERBODY, OtherBody);          // RigidBody pointer
    ATOMIC_PARAM(P_TRIGGER, Trigger);              // bool
}

}
