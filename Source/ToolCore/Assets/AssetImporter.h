
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Resource/JSONFile.h>

using namespace Atomic;

namespace ToolCore
{

class Asset;

/// deals with .asset files
class AssetImporter : public Object
{
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

    /// Imports to cached data
    virtual bool Import(const String& guid) = 0;
    virtual bool Preload() { return true; }

    Asset* GetAsset() { return asset_; }

    bool RequiresCacheFile() const { return requiresCacheFile_; }

protected:

    WeakPtr<Asset> asset_;
    JSONValue jsonRoot_;   
    bool requiresCacheFile_;


    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
