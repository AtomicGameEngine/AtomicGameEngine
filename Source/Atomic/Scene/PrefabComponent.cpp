
#include <Atomic/Core/Context.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Resource/XMLFile.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>

#include <Atomic/Physics/RigidBody.h>

#include "PrefabEvents.h"
#include "PrefabComponent.h"


namespace Atomic
{

PrefabComponent::PrefabComponent(Context* context) :
    Component(context)
{
    SubscribeToEvent(E_PREFABCHANGED, HANDLER(PrefabComponent, HandlePrefabChanged));
}

PrefabComponent::~PrefabComponent()
{

}

bool PrefabComponent::SavePrefab()
{    
    using namespace PrefabSave;
    VariantMap eventData;
    eventData[P_PREFABCOMPONENT] = this;
    SendEvent(E_PREFABSAVE, eventData);
    return true;
}

void PrefabComponent::UndoPrefab()
{
    LoadPrefabNode();
}

void PrefabComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<PrefabComponent>();

    ACCESSOR_ATTRIBUTE("PrefabGUID", GetPrefabGUID, SetPrefabGUID, String, String::EMPTY, AM_FILE | AM_NOEDIT);
}

void PrefabComponent::LoadPrefabNode()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlfile = cache->GetResource<XMLFile>(prefabGUID_);

    prefabNode_->LoadXML(xmlfile->GetRoot());
    prefabNode_->SetTemporary(true);

    prefabNode_->SetPosition(Vector3::ZERO);
    prefabNode_->SetRotation(Quaternion::IDENTITY);
    // prefabNode_->SetScale(Vector3::ONE);

    PODVector<RigidBody*> bodies;
    prefabNode_->GetComponents<RigidBody>(bodies, true);
    for (unsigned i = 0; i < bodies.Size(); i++)
    {
        RigidBody* body = bodies[i];
        body->SetTransform(body->GetNode()->GetWorldPosition(), body->GetNode()->GetWorldRotation());
    }

}

void PrefabComponent::BreakPrefab()
{
    if (!node_ || !node_->GetScene())
        return;

    SharedPtr<PrefabComponent> keepAlive(this);

    if (prefabNode_.NotNull())
        prefabNode_->SetTemporary(false);

    node_->RemoveComponent(this);


}

void PrefabComponent::HandlePrefabChanged(StringHash eventType, VariantMap& eventData)
{
    using namespace PrefabChanged;

    if (prefabGUID_ != eventData[P_GUID].GetString())
        return;

    LoadPrefabNode();

}

void PrefabComponent::SetPrefabGUID(const String& guid)
{
    assert(prefabNode_.Null());

    // ensure to use node_->CreateChild() so in scene, this may be fixed
    // with update on https://github.com/urho3d/Urho3D/issues/748
    assert(node_);

    prefabGUID_ = guid;
    prefabNode_ = node_->CreateChild();

    if (prefabGUID_.Length())
    {
        LoadPrefabNode();
    }

}

void PrefabComponent::OnNodeSet(Node* node)
{
    Component::OnNodeSet(node);


    if (!node && prefabNode_.NotNull())
    {
        // a prefab node might not be temporary is prefab is broken{
        if (prefabNode_->IsTemporary())
            prefabNode_->Remove();

        prefabNode_ = NULL;
    }
}

}
