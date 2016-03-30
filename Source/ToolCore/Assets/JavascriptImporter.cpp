//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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
