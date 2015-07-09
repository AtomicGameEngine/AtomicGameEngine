
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>
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

    SharedPtr<File> file(new File(context_, asset_->GetPath(), FILE_WRITE));
    node->SaveXML(*file);
    file->Close();

    FileSystem* fs = GetSubsystem<FileSystem>();
    fs->Copy(asset_->GetPath(), asset_->GetCachePath());

}

bool PrefabImporter::Import(const String& guid)
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
