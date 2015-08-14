
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Audio/Sound.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "AudioImporter.h"

namespace ToolCore
{

AudioImporter::AudioImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{

}

AudioImporter::~AudioImporter()
{

}

void AudioImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool AudioImporter::Import()
{
    return true;
}

bool AudioImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("AudioImporter", JSON_OBJECT);

    return true;
}

bool AudioImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("AudioImporter");

    return true;
}


Resource* AudioImporter::GetResource(const String &typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Sound* sound = cache->GetResource<Sound>(asset_->GetPath());
    return sound;
}


}
