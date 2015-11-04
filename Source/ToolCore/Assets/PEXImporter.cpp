//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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

bool PEXImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("PEXImporter");

    return true;
}

bool PEXImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("PEXImporter", import);


    return true;
}

Resource* PEXImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    ParticleEffect2D* particleEffect = cache->GetResource<ParticleEffect2D>(asset_->GetPath());

    return particleEffect;

}

}
