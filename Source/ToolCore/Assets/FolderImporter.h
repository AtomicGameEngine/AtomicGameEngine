
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class FolderImporter : public AssetImporter
{
    OBJECT(FolderImporter);

public:
    /// Construct.
    FolderImporter(Context* context);
    virtual ~FolderImporter();

    virtual void SetDefaults();

    bool Import(const String& guid);

protected:

    virtual bool LoadInternal();
    virtual bool SaveInternal();

};

}
