
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Resource/JSONFile.h>

using namespace Atomic;

namespace ToolCore
{

#define ASSET_VERSION 1

/// deals with .asset files
class AssetImporter : public Object
{
    OBJECT(AssetImporter);

public:
    /// Construct.
    AssetImporter(Context* context);
    virtual ~AssetImporter();

    bool IsDirty() { return dirty_; }

    // load .asset
    bool Load(const String& guid);
    // save .asset
    bool Save(const String& guid);

    virtual void SetDefaults();

    /// Imports to cached data
    virtual bool Import(const String& guid) = 0;

protected:

    SharedPtr<JSONFile> json_;
    bool dirty_;

    String guid_;
    unsigned timestamp_;

    virtual bool LoadInternal();
    virtual bool SaveInternal();

};

}
