
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class JavascriptImporter : public AssetImporter
{
    OBJECT(JavascriptImporter);

public:
    /// Construct.
    JavascriptImporter(Context* context, Asset* asset);
    virtual ~JavascriptImporter();

    virtual void SetDefaults();

    bool IsComponentFile() { return isComponentFile_; }

    Resource* GetResource();

protected:

    bool Import();

    bool isComponentFile_;

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
