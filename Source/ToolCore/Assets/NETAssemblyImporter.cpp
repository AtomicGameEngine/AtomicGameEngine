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
