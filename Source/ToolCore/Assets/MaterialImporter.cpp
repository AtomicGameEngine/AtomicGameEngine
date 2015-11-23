//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Material.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "MaterialImporter.h"

namespace ToolCore
{

MaterialImporter::MaterialImporter(Context* context, Asset* asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
}

MaterialImporter::~MaterialImporter()
{

}

void MaterialImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool MaterialImporter::Import()
{
    return true;
}

void MaterialImporter::SaveMaterial()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Material* material = cache->GetResource<Material>(asset_->GetPath());

    if (material)
    {
        SharedPtr<File> file(new File(context_, asset_->GetPath(), FILE_WRITE));
        material->Save(*file);
    }

}

bool MaterialImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("MaterialImporter");

    return true;
}

bool MaterialImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("MaterialImporter", import);

    return true;
}

Resource* MaterialImporter::GetResource(const String& typeName)
{
    if (!typeName.Length())
        return 0;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    return cache->GetResource(typeName, asset_->GetPath());

}



}
