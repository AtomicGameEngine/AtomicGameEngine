
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "SpriterImporter.h"

namespace ToolCore
{

SpriterImporter::SpriterImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{

}

SpriterImporter::~SpriterImporter()
{

}

void SpriterImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool SpriterImporter::Import()
{
    return true;
}

bool SpriterImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("SpriterImporter", JSON_OBJECT);

    return true;
}

bool SpriterImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("SpriterImporter");

    return true;
}


}
