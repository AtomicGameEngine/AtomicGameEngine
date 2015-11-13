//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include <AtomicJS/Javascript/JSComponentFile.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "JSONImporter.h"


namespace ToolCore
{

JSONImporter::JSONImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
}

JSONImporter::~JSONImporter()
{

}

void JSONImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool JSONImporter::Import()
{
    return true;
}

bool JSONImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("JSONImporter");

    return true;
}

bool JSONImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import;
    jsonRoot.Set("JSONImporter", import);

    return true;
}

Resource* JSONImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    JSONFile* jsfile = cache->GetResource<JSONFile>(asset_->GetPath());

    return jsfile;

}



}
