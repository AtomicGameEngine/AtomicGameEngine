//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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


#include <Atomic/IO/MemoryBuffer.h>
#include <Atomic/Physics/PhysicsEvents.h>
#include <Atomic/Scene/Node.h>
#include "ScriptPhysics.h"


namespace Atomic
{

void PhysicsNodeCollision::SetFromNodeCollisionEvent(VariantMap& eventData)
{
    using namespace NodeCollision;

    body_ = static_cast<RigidBody*>(eventData[P_BODY].GetPtr());

    otherNode_ = static_cast<Node*>(eventData[P_OTHERNODE].GetPtr());
    otherBody_ = static_cast<RigidBody*>(eventData[P_OTHERBODY].GetPtr());

    trigger_ = eventData[P_TRIGGER].GetBool();
    
    MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

    while (!contacts.IsEof())
    {
        contacts_.Push(SharedPtr<PhysicsContact>(new PhysicsContact(contacts.ReadVector3(), contacts.ReadVector3(), contacts.ReadFloat(), contacts.ReadFloat())));
    }
    
}

}