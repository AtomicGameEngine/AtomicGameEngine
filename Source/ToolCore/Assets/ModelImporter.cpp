
#include "../Import/OpenAssetImporter.h"

#include "Asset.h"
#include "AssetDatabase.h"
#include "ModelImporter.h"

namespace ToolCore
{

/// Node + Model (static or animated)
ModelImporter::ModelImporter(Context* context) : AssetImporter(context)
{

}

ModelImporter::~ModelImporter()
{

}

void ModelImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool ModelImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

    SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));

    //importer->SetVerboseLog(true);

    if (importer->Load(asset->GetPath()))
    {
        String cachePath = db->GetCachePath();

        cachePath += guid;

        importer->ExportModel(cachePath);
    }
    else
    {
        return false;
    }

    return true;
}

bool ModelImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("ModelImporter", JSON_OBJECT);

    return true;
}

bool ModelImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("ModelImporter");

    return true;
}


}
