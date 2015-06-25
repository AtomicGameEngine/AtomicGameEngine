
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "PrefabImporter.h"

namespace ToolCore
{

PrefabImporter::PrefabImporter(Context* context) : AssetImporter(context)
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

bool PrefabImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

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
