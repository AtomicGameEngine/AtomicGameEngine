
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class TextureImporter : public AssetImporter
{
    OBJECT(TextureImporter);

public:
    /// Construct.
    TextureImporter(Context* context, Asset* asset);
    virtual ~TextureImporter();

    virtual void SetDefaults();

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
