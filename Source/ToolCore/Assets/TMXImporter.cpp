
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

bool TMXImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("TMXImporter", JSON_OBJECT);

    return true;
}

bool TMXImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("TMXImporter");

    return true;
}


}
