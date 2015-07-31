
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

    Node* node = component->GetPrefabNode();

    node->SetPosition(Vector3::ZERO);
    node->SetRotation(Quaternion::IDENTITY);
    node->SetScale(Vector3::ONE);

    SharedPtr<File> file(new File(context_, asset_->GetPath(), FILE_WRITE));
    node->SaveXML(*file);
    file->Close();

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


}
