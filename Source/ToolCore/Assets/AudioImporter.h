
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class AudioImporter : public AssetImporter
{
    OBJECT(AudioImporter);

public:
    /// Construct.
    AudioImporter(Context* context, Asset* asset);
    virtual ~AudioImporter();

    virtual void SetDefaults();

    Resource* GetResource(const String& typeName = String::EMPTY);

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
