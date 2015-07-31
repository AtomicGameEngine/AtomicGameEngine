
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class SceneImporter : public AssetImporter
{
    OBJECT(SceneImporter);

public:
    /// Construct.
    SceneImporter(Context* context, Asset *asset);
    virtual ~SceneImporter();

    virtual void SetDefaults();

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
