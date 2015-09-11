
#include "Asset.h"
#include "AssetDatabase.h"
#include "SceneImporter.h"

namespace ToolCore
{

SceneImporter::SceneImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{

}

SceneImporter::~SceneImporter()
{

}

void SceneImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool SceneImporter::Import()
{
    return true;
}

bool SceneImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("SceneImporter");

    return true;
}

bool SceneImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("SceneImporter", import);

    return true;
}


}
