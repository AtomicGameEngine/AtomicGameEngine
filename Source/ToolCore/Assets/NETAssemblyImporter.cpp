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

#include <AtomicNET/NETCore/NETAssemblyFile.h>

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

    JSONValue assemblyJSON;

    if (tools->InspectAssembly(asset_->GetPath(), assemblyJSON))
    {
        if (assemblyJSON.IsObject())
        {
            const JSONValue& enums = assemblyJSON.Get("enums");

            const JSONArray& components = assemblyJSON.Get("components").GetArray();

            for (unsigned i = 0; i < components.Size(); i++)
            {
                const JSONValue& c = components.At(i);

                String componentName = c.Get("name").GetString();

                LOGINFOF("Found NET Component %s", componentName.CString());
            }

        }

    }

    return true;
}

bool NETAssemblyImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("NETAssemblyImporter");

    //isComponentFile_ = import.Get("IsComponentFile").GetBool();

    return true;
}

bool NETAssemblyImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import;
    //import.Set("IsComponentFile", JSONValue(isComponentFile_));
    jsonRoot.Set("NETAssemblyImporter", import);

    return true;
}

Resource* NETAssemblyImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    NETAssemblyFile* assemblyFile = cache->GetResource<NETAssemblyFile>(asset_->GetPath());

    return assemblyFile;

}



}
