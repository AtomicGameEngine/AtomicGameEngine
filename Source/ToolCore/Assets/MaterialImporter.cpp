
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Material.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "MaterialImporter.h"

namespace ToolCore
{

MaterialImporter::MaterialImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
}

MaterialImporter::~MaterialImporter()
{

}

void MaterialImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool MaterialImporter::Import()
{
    return true;
}

void MaterialImporter::SaveMaterial()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Material* material = cache->GetResource<Material>(asset_->GetPath());

    if (material)
    {
        SharedPtr<File> file(new File(context_, asset_->GetPath(), FILE_WRITE));
        material->Save(*file);
    }

}

bool MaterialImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("MaterialImporter", JSON_OBJECT);

    return true;
}

bool MaterialImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("MaterialImporter");

    return true;
}


}
