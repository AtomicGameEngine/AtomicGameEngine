
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

bool FolderImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("FolderImporter", JSON_OBJECT);

    return true;
}

bool FolderImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("FolderImporter");

    return true;
}


}
