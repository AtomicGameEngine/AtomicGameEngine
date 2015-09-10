
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class TextImporter : public AssetImporter
{
    OBJECT(TextImporter);

public:
    /// Construct.
    TextImporter(Context* context, Asset* asset);
    virtual ~TextImporter();

    virtual void SetDefaults();

protected:

    bool Import();

    virtual bool LoadSettingsInternal(JSONValue& jsonRoot);
    virtual bool SaveSettingsInternal(JSONValue& jsonRoot);

};

}
