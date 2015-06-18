
#include "Asset.h"
#include "AssetDatabase.h"
#include "MaterialImporter.h"

namespace ToolCore
{

MaterialImporter::MaterialImporter(Context* context) : AssetImporter(context)
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

bool MaterialImporter::LoadInternal()
{
    if (!AssetImporter::LoadInternal())
        return false;

    JSONValue root = json_->GetRoot();

    JSONValue import = root.GetChild("MaterialImporter", JSON_OBJECT);

    return true;
}

bool MaterialImporter::SaveInternal()
{
    if (!AssetImporter::SaveInternal())
        return false;

    JSONValue root = json_->GetRoot();

    JSONValue import = root.CreateChild("MaterialImporter");

    return true;
}


}
