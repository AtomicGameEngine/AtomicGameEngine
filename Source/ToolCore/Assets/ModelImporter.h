
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class ModelImporter : public AssetImporter
{
    OBJECT(ModelImporter);

public:
    /// Construct.
    ModelImporter(Context* context);
    virtual ~ModelImporter();

    virtual void SetDefaults();

    bool Import(const String& guid);

protected:

    virtual bool LoadInternal();
    virtual bool SaveInternal();

};

}
