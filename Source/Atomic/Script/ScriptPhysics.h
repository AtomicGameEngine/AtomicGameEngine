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


#pragma once


#include <Atomic/Core/Variant.h>
#include <Atomic/Physics/RigidBody.h>
#include <Atomic/Scene/Node.h>

namespace Atomic
{
    class Node;

    class PhysicsContact : public RefCounted
    {
        friend class PhysicsNodeCollision;

        ATOMIC_REFCOUNTED(PhysicsNodeContact)

    public:

        PhysicsContact() {};

        const Vector3& GetPosition() const { return position_; }
        const Vector3& GetNormal() const { return normal_; }
        float GetDistance() const { return distance_; }
        float GetImpulse() const { return impulse_; }

    private:

        PhysicsContact(const Vector3& position, const Vector3& normal, float distance, float impulse) :
            position_(position),
            normal_(normal),
            distance_(distance),
            impulse_(impulse)
        {

        }

        Vector3 position_;
        Vector3 normal_;
        float distance_;
        float impulse_;
    };

    class PhysicsNodeCollision : public RefCounted
    {
        ATOMIC_REFCOUNTED(PhysicsNodeCollision)

    public:

        PhysicsNodeCollision() : trigger_(false)
        {

        }

        RigidBody* GetBody() const { return body_; }

        Node* GetOtherNode() const { return otherNode_; }
        RigidBody* GetOtherBody() const { return otherBody_; }

        bool GetTrigger() const { return trigger_; }

        const Vector<SharedPtr<PhysicsContact>>& GetContacts() const { return contacts_;  }

        void SetFromNodeCollisionEvent(VariantMap& eventData);

    private:

        SharedPtr<RigidBody> body_;

        SharedPtr<Node> otherNode_;
        SharedPtr<RigidBody> otherBody_;
        bool trigger_;

        Vector<SharedPtr<PhysicsContact>> contacts_;

    };
}