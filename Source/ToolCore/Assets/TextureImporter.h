
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class TextureImporter : public AssetImporter
{
    OBJECT(TextureImporter);

public:
    /// Construct.
    TextureImporter(Context* context);
    virtual ~TextureImporter();

    virtual void SetDefaults();

    bool Import(const String& guid);

protected:

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
