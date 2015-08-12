
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

    virtual Resource* GetResource() { return 0; }

    bool RequiresCacheFile() const { return requiresCacheFile_; }

protected:

    virtual bool Import() { return true; }

    WeakPtr<Asset> asset_;
    JSONValue jsonRoot_;   
    bool requiresCacheFile_;


    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
