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
#include "TypeScriptImporter.h"


namespace ToolCore
{

TypeScriptImporter::TypeScriptImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
    isComponentFile_ = false;
}

TypeScriptImporter::~TypeScriptImporter()
{

}

void TypeScriptImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool TypeScriptImporter::Import()
{
    isComponentFile_ = false;

    return true;
}

bool TypeScriptImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("TypeScriptImporter");

    isComponentFile_ = import.Get("IsComponentFile").GetBool();

    return true;
}

bool TypeScriptImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import;
    import.Set("IsComponentFile", JSONValue(isComponentFile_));
    jsonRoot.Set("TypeScriptImporter", import);

    return true;
}

Resource* TypeScriptImporter::GetResource(const String& typeName)
{
    if (!isComponentFile_)
        return 0;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    JSComponentFile* jsc = cache->GetResource<JSComponentFile>(asset_->GetPath());

    return jsc;

}



}
