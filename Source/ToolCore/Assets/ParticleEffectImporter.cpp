//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>
#include <Atomic/Atomic3D/ParticleEffect.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "ParticleEffectImporter.h"

namespace ToolCore
{

ParticleEffectImporter::ParticleEffectImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{

}

ParticleEffectImporter::~ParticleEffectImporter()
{

}

void ParticleEffectImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool ParticleEffectImporter::Import()
{
    return true;
}

bool ParticleEffectImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("ParticleEffectImporter");

    return true;
}

bool ParticleEffectImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("ParticleEffectImporter", import);


    return true;
}

Resource* ParticleEffectImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    ParticleEffect* particleEffect = cache->GetResource<ParticleEffect>(asset_->GetPath());

    return particleEffect;

}

}
