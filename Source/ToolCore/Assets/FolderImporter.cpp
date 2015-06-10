
#include "Asset.h"
#include "AssetDatabase.h"
#include "FolderImporter.h"

namespace ToolCore
{

FolderImporter::FolderImporter(Context* context) : AssetImporter(context)
{

}

FolderImporter::~FolderImporter()
{

}

void FolderImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool FolderImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

    return true;
}

bool FolderImporter::LoadInternal()
{
    if (!AssetImporter::LoadInternal())
        return false;

    JSONValue root = json_->GetRoot();

    JSONValue import = root.GetChild("FolderImporter", JSON_OBJECT);

    return true;
}

bool FolderImporter::SaveInternal()
{
    if (!AssetImporter::SaveInternal())
        return false;

    JSONValue root = json_->GetRoot();

    JSONValue import = root.CreateChild("FolderImporter");

    return true;
}


}
