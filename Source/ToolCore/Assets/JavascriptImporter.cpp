
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "JavascriptImporter.h"

namespace ToolCore
{

JavascriptImporter::JavascriptImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
}

JavascriptImporter::~JavascriptImporter()
{

}

void JavascriptImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool JavascriptImporter::Import(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);

    if (!asset)
        return false;

    return true;
}

bool JavascriptImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("JavascriptImporter", JSON_OBJECT);

    return true;
}

bool JavascriptImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("JavascriptImporter");

    return true;
}


}
