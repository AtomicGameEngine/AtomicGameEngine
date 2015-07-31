
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

    void SaveMaterial();

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
