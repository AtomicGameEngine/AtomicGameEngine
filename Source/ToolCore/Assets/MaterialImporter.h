
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class MaterialImporter : public AssetImporter
{
    OBJECT(MaterialImporter);

public:
    /// Construct.
    MaterialImporter(Context* context, Asset *asset);
    virtual ~MaterialImporter();

    virtual void SetDefaults();

    bool Import(const String& guid);

protected:

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
