
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

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
