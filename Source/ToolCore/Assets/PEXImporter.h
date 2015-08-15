
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class PEXImporter : public AssetImporter
{
    OBJECT(PEXImporter);

public:
    /// Construct.
    PEXImporter(Context* context, Asset* asset);
    virtual ~PEXImporter();

    virtual void SetDefaults();

    Resource* GetResource(const String& typeName);

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
