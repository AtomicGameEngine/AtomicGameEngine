
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class SpriterImporter : public AssetImporter
{
    OBJECT(SpriterImporter);

public:
    /// Construct.
    SpriterImporter(Context* context, Asset* asset);
    virtual ~SpriterImporter();

    virtual void SetDefaults();

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
