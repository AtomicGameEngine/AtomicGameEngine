
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

    bool Import(const String& guid);

protected:

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
