
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