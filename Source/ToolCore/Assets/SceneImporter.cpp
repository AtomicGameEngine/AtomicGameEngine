
#include "Asset.h"
#include "AssetDatabase.h"
#include "SceneImporter.h"

namespace ToolCore
{

SceneImporter::SceneImporter(Context* context) : AssetImporter(context)
{

}

SceneImporter::~SceneImporter()
{

}

void SceneImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool SceneImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

    return true;
}

bool SceneImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("SceneImporter", JSON_OBJECT);

    return true;
}

bool SceneImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("SceneImporter");

    return true;
}


}
