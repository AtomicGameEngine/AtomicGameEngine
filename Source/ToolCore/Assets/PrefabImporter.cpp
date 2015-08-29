
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/XMLFile.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/PrefabEvents.h>
#include <Atomic/Scene/PrefabComponent.h>
#include <Atomic/Atomic2D/AnimatedSprite2D.h>
#include <Atomic/IO/FileSystem.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "PrefabImporter.h"

namespace ToolCore
{

PrefabImporter::PrefabImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{
    SubscribeToEvent(E_PREFABSAVE, HANDLER(PrefabImporter, HandlePrefabSave));
}

PrefabImporter::~PrefabImporter()
{

}

void PrefabImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool PrefabImporter::Preload()
{
    if (!asset_)
        return false;

    preloadResourceScene_ = new Scene(context_);
    SharedPtr<File> file(new File(context_, asset_->GetCachePath()));
    preloadResourceScene_->LoadAsyncXML(file, LOAD_RESOURCES_ONLY);

    return true;
}

void PrefabImporter::HandlePrefabSave(StringHash eventType, VariantMap& eventData)
{
    using namespace PrefabSave;

    PrefabComponent* component = static_cast<PrefabComponent*>(eventData[P_PREFABCOMPONENT].GetPtr());

    if (component->GetPrefabGUID() != asset_->GetGUID())
        return;

    Node* node = component->GetNode();

    if (!node)
        return;

    // flip temporary root children and components to not be temporary for save
    const Vector<SharedPtr<Component>>& rootComponents = node->GetComponents();
    const Vector<SharedPtr<Node> >& children = node->GetChildren();

    PODVector<Component*> tempComponents;
    PODVector<Node*> tempChildren;
    PODVector<Node*> filterNodes;

    for (unsigned i = 0; i < rootComponents.Size(); i++)
    {
        if (rootComponents[i]->IsTemporary())
        {
            rootComponents[i]->SetTemporary(false);
            tempComponents.Push(rootComponents[i]);

            // Animated sprites contain a temporary node we don't want to save in the prefab
            // it would be nice if this was general purpose because have to test this when
            // breaking node as well
            if (rootComponents[i]->GetType() == AnimatedSprite2D::GetTypeStatic())
            {
                AnimatedSprite2D* asprite = (AnimatedSprite2D*) rootComponents[i].Get();
                if (asprite->GetRootNode())
                    filterNodes.Push(asprite->GetRootNode());
            }

        }
    }

    for (unsigned i = 0; i < children.Size(); i++)
    {
        if (filterNodes.Contains(children[i].Get()))
            continue;

        if (children[i]->IsTemporary())
        {
            children[i]->SetTemporary(false);
            tempChildren.Push(children[i]);
        }
    }

    // store original transform
    Vector3 pos = node->GetPosition();
    Quaternion rot = node->GetRotation();
    Vector3 scale = node->GetScale();

    node->SetPosition(Vector3::ZERO);
    node->SetRotation(Quaternion::IDENTITY);
    node->SetScale(Vector3::ONE);

    component->SetTemporary(true);

    SharedPtr<File> file(new File(context_, asset_->GetPath(), FILE_WRITE));
    node->SaveXML(*file);
    file->Close();

    component->SetTemporary(false);

    // restore
    node->SetPosition(pos);
    node->SetRotation(rot);
    node->SetScale(scale);

    for (unsigned i = 0; i < tempComponents.Size(); i++)
    {
        tempComponents[i]->SetTemporary(true);
    }

    for (unsigned i = 0; i < tempChildren.Size(); i++)
    {
        tempChildren[i]->SetTemporary(true);
    }


    FileSystem* fs = GetSubsystem<FileSystem>();
    fs->Copy(asset_->GetPath(), asset_->GetCachePath());

    // reload it immediately so it is ready for use
    // TODO: The resource cache is reloading after this reload due to catching the file cache
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlfile = cache->GetResource<XMLFile>(asset_->GetGUID());
    cache->ReloadResource(xmlfile);

    VariantMap changedData;
    changedData[PrefabChanged::P_GUID] = asset_->GetGUID();
    SendEvent(E_PREFABCHANGED, changedData);

}

bool PrefabImporter::Import()
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    fs->Copy(asset_->GetPath(), asset_->GetCachePath());

    return true;
}

bool PrefabImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("PrefabImporter", JSON_OBJECT);

    return true;
}

bool PrefabImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("PrefabImporter");

    return true;
}

Node* PrefabImporter::InstantiateNode(Node* parent, const String& name)
{
    Node* node = parent->CreateChild(asset_->GetName());
    PrefabComponent* pc = node->CreateComponent<PrefabComponent>();
    pc->SetPrefabGUID(asset_->GetGUID());
    return node;
}

}
