
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class TMXImporter : public AssetImporter
{
    OBJECT(TMXImporter);

public:
    /// Construct.
    TMXImporter(Context* context, Asset* asset);
    virtual ~TMXImporter();

    virtual void SetDefaults();

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
