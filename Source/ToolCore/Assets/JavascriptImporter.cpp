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
#include "JavascriptImporter.h"


namespace ToolCore
{

JavascriptImporter::JavascriptImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
    isComponentFile_ = false;
}

JavascriptImporter::~JavascriptImporter()
{

}

void JavascriptImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool JavascriptImporter::Import()
{
    isComponentFile_ = false;

    const String& path = asset_->GetPath();

    SharedPtr<File> file(new File(context_, path, FILE_READ));

    unsigned dataSize = file->GetSize();

    SharedArrayPtr<char> buffer(new char[dataSize + 1]);

    if (file->Read(buffer.Get(), dataSize) != dataSize)
        return false;

    buffer[dataSize] = '\0';

    file->Close();

    if (strstr(buffer, "\"atomic component\"") || strstr(buffer, "'atomic component'"))
        isComponentFile_ = true;

    return true;
}

bool JavascriptImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("JavascriptImporter");

    isComponentFile_ = import.Get("IsComponentFile").GetBool();

    return true;
}

bool JavascriptImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import;
    import.Set("IsComponentFile", JSONValue(isComponentFile_));
    jsonRoot.Set("JavascriptImporter", import);

    return true;
}

Resource* JavascriptImporter::GetResource(const String& typeName)
{
    if (!isComponentFile_)
        return 0;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    JSComponentFile* jsc = cache->GetResource<JSComponentFile>(asset_->GetPath());

    return jsc;

}



}
