
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