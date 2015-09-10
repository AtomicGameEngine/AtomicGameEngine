
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include "AssetDatabase.h"
#include "AssetImporter.h"

namespace ToolCore
{

AssetImporter::AssetImporter(Context* context, Asset *asset) : Object(context),
    asset_(asset),
    requiresCacheFile_(true)
{
    SetDefaults();
}

AssetImporter::~AssetImporter()
{

}

void AssetImporter::SetDefaults()
{

}

bool AssetImporter::LoadSettings(JSONValue& root)
{
    LoadSettingsInternal(root);
    return true;
}

bool AssetImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    return true;
}

bool AssetImporter::SaveSettings(JSONValue& root)
{
    SaveSettingsInternal(root);
    return true;
}

bool AssetImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    return true;
}


}
