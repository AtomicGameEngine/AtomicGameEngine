
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>
#include <Atomic/Atomic2D/ParticleEffect2D.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "PEXImporter.h"

namespace ToolCore
{

PEXImporter::PEXImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{

}

PEXImporter::~PEXImporter()
{

}

void PEXImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool PEXImporter::Import()
{
    return true;
}

bool PEXImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("PEXImporter", JSON_OBJECT);

    return true;
}

bool PEXImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("PEXImporter");

    return true;
}

Resource* PEXImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    ParticleEffect2D* particleEffect = cache->GetResource<ParticleEffect2D>(asset_->GetPath());

    return particleEffect;

}

}
