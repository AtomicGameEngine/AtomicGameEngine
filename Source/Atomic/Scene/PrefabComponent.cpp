
#include <Atomic/Core/Context.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Resource/XMLFile.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>

#include "PrefabEvents.h"
#include "PrefabComponent.h"


namespace Atomic
{

PrefabComponent::PrefabComponent(Context* context) :
    Component(context)
{

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

void PrefabComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<PrefabComponent>();

    ACCESSOR_ATTRIBUTE("PrefabGUID", GetPrefabGUID, SetPrefabGUID, String, String::EMPTY, AM_FILE | AM_NOEDIT);
}

void PrefabComponent::HandleReloadFinished(StringHash eventType, VariantMap& eventData)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlfile = cache->GetResource<XMLFile>(prefabGUID_);

    prefabNode_->LoadXML(xmlfile->GetRoot());
    prefabNode_->SetTemporary(true);

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
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        XMLFile* xmlfile = cache->GetResource<XMLFile>(prefabGUID_);

        SubscribeToEvent(xmlfile, E_RELOADFINISHED, HANDLER(PrefabComponent, HandleReloadFinished));

        if (xmlfile)
        {
            prefabNode_->LoadXML(xmlfile->GetRoot());
        }
    }

    prefabNode_->SetName(prefabNode_->GetName() + "_Prefab");
    prefabNode_->SetTemporary(true);

}

void PrefabComponent::OnNodeSet(Node* node)
{
    Component::OnNodeSet(node);
}

}
