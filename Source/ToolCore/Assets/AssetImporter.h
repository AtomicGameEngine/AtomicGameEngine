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

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Resource/JSONFile.h>
#include <Atomic/Scene/Node.h>

using namespace Atomic;

namespace ToolCore
{

class Asset;

/// deals with .asset files
class AssetImporter : public Object
{
    friend class Asset;

    ATOMIC_OBJECT(AssetImporter, Object);

public:
    /// Construct.
    AssetImporter(Context* context, Asset* asset);
    virtual ~AssetImporter();

    // load .asset
    bool LoadSettings(JSONValue& root);
    // save .asset
    bool SaveSettings(JSONValue& root);

    virtual void SetDefaults();

    virtual bool Preload() { return true; }

    Asset* GetAsset() { return asset_; }

    virtual Resource* GetResource(const String& typeName = String::EMPTY) { return 0; }

    bool RequiresCacheFile() const { return requiresCacheFile_; }

    /// Instantiate a node from the asset
    virtual Node* InstantiateNode(Node* parent, const String& name) { return 0; }

    virtual bool Rename(const String& newName);
    virtual bool Move(const String& newPath);

protected:

    virtual bool Import() { return true; }

    WeakPtr<Asset> asset_;
    bool requiresCacheFile_;


    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

};

}
