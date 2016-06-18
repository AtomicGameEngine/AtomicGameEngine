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

#include <AtomicNET/NETScript/CSComponentAssembly.h>

#include "../NETTools/NETToolSystem.h"

#include "Asset.h"
#include "AssetDatabase.h"
#include "NETAssemblyImporter.h"

namespace ToolCore
{

NETAssemblyImporter::NETAssemblyImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
}

NETAssemblyImporter::~NETAssemblyImporter()
{

}

void NETAssemblyImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool NETAssemblyImporter::Import()
{
    NETToolSystem* tools = GetSubsystem<NETToolSystem>();

    assemblyJSON_.SetType(JSON_NULL);

    if (tools->InspectAssembly(asset_->GetPath(), assemblyJSON_))
    {
        if (!assemblyJSON_.IsObject())
        {
            assemblyJSON_.SetType(JSON_NULL);
        }
        else
        {
            ResourceCache* cache = GetSubsystem<ResourceCache>();
            CSComponentAssembly* assemblyFile = cache->GetResource<CSComponentAssembly>(asset_->GetPath());
            if (assemblyFile)
                assemblyFile->ParseAssemblyJSON(assemblyJSON_);
        }

    }

    return true;
}

bool NETAssemblyImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("NETAssemblyImporter");

    assemblyJSON_.SetType(JSON_NULL);

    const JSONValue& ajson = import.Get("AssemblyJSON");

    if (ajson.IsObject())
    {
        assemblyJSON_ = ajson.GetObject();

        ResourceCache* cache = GetSubsystem<ResourceCache>();
        CSComponentAssembly* assemblyFile = cache->GetResource<CSComponentAssembly>(asset_->GetPath());
        if (assemblyFile)
            assemblyFile->ParseAssemblyJSON(assemblyJSON_);
    }

    return true;
}

bool NETAssemblyImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import;
    import.SetType(JSON_OBJECT);
    import.Set("AssemblyJSON", assemblyJSON_);
    jsonRoot.Set("NETAssemblyImporter", import);

    return true;
}

Resource* NETAssemblyImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    CSComponentAssembly* assemblyFile = cache->GetResource<CSComponentAssembly>(asset_->GetPath());

    return assemblyFile;

}



}
