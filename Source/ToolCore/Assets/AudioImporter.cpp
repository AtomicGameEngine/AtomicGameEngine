//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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

bool AudioImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("AudioImporter");

    return true;
}

bool AudioImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("AudioImporter", import);

    return true;
}


Resource* AudioImporter::GetResource(const String &typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Sound* sound = cache->GetResource<Sound>(asset_->GetPath());
    return sound;
}


}
