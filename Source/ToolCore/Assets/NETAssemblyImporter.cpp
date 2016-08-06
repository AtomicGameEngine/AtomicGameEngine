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

#include <Atomic/Core/Variant.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include <AtomicNET/NETScript/NETScriptEvents.h>
#include <AtomicNET/NETScript/CSComponentAssembly.h>

#include "Asset.h"
#include "AssetDatabase.h"

#include "../NETTools/AtomicNETService.h"

#include "NETAssemblyImporter.h"

namespace ToolCore
{

    NETAssemblyImporter::NETAssemblyImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
    {
        requiresCacheFile_ = false;
        resultHandler_ = new NETAssemblyImporterResultHandler(context, this);
    }

    NETAssemblyImporter::~NETAssemblyImporter()
    {

    }

    void NETAssemblyImporter::SetDefaults()
    {
        AssetImporter::SetDefaults();
    }

    void NETAssemblyImporter::HandleResult(unsigned cmdID, const VariantMap& cmdResult)
    {
        const String& command = cmdResult["command"]->GetString();

        if (command == "parse")
        {
            // Assembly parse results
            const String& resultJSON = cmdResult["result"]->GetString();

            assemblyJSON_.SetType(JSON_NULL);

            JSONFile::ParseJSON(resultJSON, assemblyJSON_);

            if (!assemblyJSON_.IsObject())
            {
                ATOMIC_LOGERRORF("NETAssemblyImporter::HandleResult - Unable to parse assembly json for %s", asset_->GetPath().CString());
            }
            else
            {
                ResourceCache* cache = GetSubsystem<ResourceCache>();
                CSComponentAssembly* assemblyFile = cache->GetResource<CSComponentAssembly>(asset_->GetPath());
                if (assemblyFile)
                {
                    if (!assemblyFile->ParseAssemblyJSON(assemblyJSON_))
                    {
                        ATOMIC_LOGERRORF("NETAssemblyImporter::HandleResult - Unable to parse assembly %s", asset_->GetPath().CString());
                    }
                    else
                    {
                        asset_->Save();

                        using namespace CSComponentAssemblyChanged;
                        VariantMap assemblyChangedEventData;
                        assemblyChangedEventData[P_RESOURCE] = asset_->GetResource();
                        assemblyChangedEventData[P_ASSEMBLYPATH] = asset_->GetPath();        
                        SendEvent(E_CSCOMPONENTASSEMBLYCHANGED, assemblyChangedEventData);
                    }
                }
                else
                {

                    ATOMIC_LOGERRORF("NETAssemblyImporter::HandleResult - Unable to get CSComponentAssembly resource for %s", asset_->GetPath().CString());

                }
                    
            }
            
        }

    }

    bool NETAssemblyImporter::Import()
    {
        AtomicNETService* service = GetSubsystem<AtomicNETService>();

        if (!service)
        {
            ATOMIC_LOGERRORF("NETAssemblyImporter::Import - Unable to get AtomicNETService subsystem importing %s", asset_->GetPath().CString());
            return false;
        }


        assemblyJSON_.SetType(JSON_NULL);

        VariantMap cmdMap;
        cmdMap["command"] = "parse";
        cmdMap["assemblyPath"] = asset_->GetPath();

        service->QueueCommand(resultHandler_, cmdMap);

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


    // NETAssemblyImporterClient


    NETAssemblyImporterResultHandler::NETAssemblyImporterResultHandler(Context* context, NETAssemblyImporter* importer) :
        IPCResultHandler(context),
        importer_(importer)
    {

    }

    NETAssemblyImporterResultHandler::~NETAssemblyImporterResultHandler()
    {

    }

    void NETAssemblyImporterResultHandler::HandleResult(unsigned cmdID, const VariantMap& cmdResult)
    {
        importer_->HandleResult(cmdID, cmdResult);
    }

}
