
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

    Resource* GetResource(const String& typeName = String::EMPTY);

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
