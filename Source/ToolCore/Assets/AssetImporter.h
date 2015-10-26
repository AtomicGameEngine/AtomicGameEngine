//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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

    OBJECT(AssetImporter);

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
