
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class SceneImporter : public AssetImporter
{
    OBJECT(SceneImporter);

public:
    /// Construct.
    SceneImporter(Context* context);
    virtual ~SceneImporter();

    virtual void SetDefaults();

    bool Import(const String& guid);

protected:

    virtual bool LoadInternal();
    virtual bool SaveInternal();

};

}
