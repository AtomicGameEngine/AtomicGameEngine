
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include <Atomic/Scene/Scene.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "PrefabImporter.h"

namespace ToolCore
{

PrefabImporter::PrefabImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
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
    SharedPtr<File> file(new File(context_, asset_->GetPath()));
    preloadResourceScene_->LoadAsyncXML(file, LOAD_RESOURCES_ONLY);

    return true;
}

bool PrefabImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

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
