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

#include "../ToolSystem.h"
#include "../Project/Project.h"

#include "Asset.h"
#include "AssetDatabase.h"
#include "CSharpImporter.h"


namespace ToolCore
{

    CSharpImporter::CSharpImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
    {
        requiresCacheFile_ = false;
    }

    CSharpImporter::~CSharpImporter()
    {

    }

    void CSharpImporter::SetDefaults()
    {
        AssetImporter::SetDefaults();
    }

    bool CSharpImporter::Import()
    {
        return true;
    }

    bool CSharpImporter::LoadSettingsInternal(JSONValue& jsonRoot)
    {
        if (!AssetImporter::LoadSettingsInternal(jsonRoot))
            return false;

        JSONValue import = jsonRoot.Get("CSharpImporter");       
        return true;
    }

    bool CSharpImporter::SaveSettingsInternal(JSONValue& jsonRoot)
    {
        if (!AssetImporter::SaveSettingsInternal(jsonRoot))
            return false;

        JSONValue import;
        jsonRoot.Set("CSharpImporter", import);

        return true;
    }

    Resource* CSharpImporter::GetResource(const String& typeName)
    {
        return 0;

    }

}
