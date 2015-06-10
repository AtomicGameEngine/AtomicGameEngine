
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

bool SceneImporter::LoadInternal()
{
    if (!AssetImporter::LoadInternal())
        return false;

    JSONValue root = json_->GetRoot();

    JSONValue import = root.GetChild("SceneImporter", JSON_OBJECT);

    return true;
}

bool SceneImporter::SaveInternal()
{
    if (!AssetImporter::SaveInternal())
        return false;

    JSONValue root = json_->GetRoot();

    JSONValue import = root.CreateChild("SceneImporter");

    return true;
}


}
