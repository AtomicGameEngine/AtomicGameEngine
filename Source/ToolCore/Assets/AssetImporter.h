
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Resource/JSONFile.h>

using namespace Atomic;

namespace ToolCore
{

/// deals with .asset files
class AssetImporter : public Object
{
    OBJECT(AssetImporter);

public:
    /// Construct.
    AssetImporter(Context* context);
    virtual ~AssetImporter();

    // load .asset
    bool LoadSettings(JSONValue& root);
    // save .asset
    bool SaveSettings(JSONValue& root);

    virtual void SetDefaults();

    /// Imports to cached data
    virtual bool Import(const String& guid) = 0;

protected:

    JSONValue jsonRoot_;

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
