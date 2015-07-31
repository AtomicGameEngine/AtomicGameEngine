
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class FolderImporter : public AssetImporter
{
    OBJECT(FolderImporter);

public:
    /// Construct.
    FolderImporter(Context* context, Asset* asset);
    virtual ~FolderImporter();

    virtual void SetDefaults();

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
