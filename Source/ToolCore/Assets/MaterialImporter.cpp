
#include "Asset.h"
#include "AssetDatabase.h"
#include "MaterialImporter.h"

namespace ToolCore
{

MaterialImporter::MaterialImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{

}

MaterialImporter::~MaterialImporter()
{

}

void MaterialImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool MaterialImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

    return true;
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
