
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "TMXImporter.h"

namespace ToolCore
{

TMXImporter::TMXImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{

}

TMXImporter::~TMXImporter()
{

}

void TMXImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool TMXImporter::Import()
{
    return true;
}

bool TMXImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("TMXImporter");

    return true;
}

bool TMXImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);


    jsonRoot.Set("TMXImporter", import);

    return true;
}


}
