
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

    bool Import(const String& guid);

protected:

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
