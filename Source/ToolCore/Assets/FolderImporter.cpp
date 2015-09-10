
#include "Asset.h"
#include "AssetDatabase.h"
#include "FolderImporter.h"

namespace ToolCore
{

FolderImporter::FolderImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{

}

FolderImporter::~FolderImporter()
{

}

void FolderImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool FolderImporter::Import()
{
    return true;
}

bool FolderImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("FolderImporter");

    return true;
}

bool FolderImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);


    jsonRoot.Set("FolderImporter", import);

    return true;
}


}
