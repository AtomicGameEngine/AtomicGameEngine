//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/XMLFile.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/PrefabEvents.h>
#include <Atomic/Scene/PrefabComponent.h>
#include <Atomic/IO/FileSystem.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "PrefabImporter.h"

namespace ToolCore
{

PrefabImporter::PrefabImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{
    SubscribeToEvent(E_PREFABSAVE, ATOMIC_HANDLER(PrefabImporter, HandlePrefabSave));
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

    OnPrefabFileChanged();

}

bool PrefabImporter::Import()
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    fs->Copy(asset_->GetPath(), asset_->GetCachePath());

    OnPrefabFileChanged();

    return true;
}

void PrefabImporter::OnPrefabFileChanged()
{
    // reload it immediately so it is ready for use
    // TODO: The resource cache is reloading after this reload due to catching the file cache
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlfile = cache->GetResource<XMLFile>(asset_->GetGUID());
    cache->ReloadResource(xmlfile);

    VariantMap changedData;
    changedData[PrefabChanged::P_GUID] = asset_->GetGUID();
    SendEvent(E_PREFABCHANGED, changedData);
}

bool PrefabImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("PrefabImporter");

    return true;
}

bool PrefabImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("PrefabImporter", import);

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
