
#pragma once

#include "AssetImporter.h"

namespace ToolCore
{

class PrefabImporter : public AssetImporter
{
    OBJECT(PrefabImporter);

public:
    /// Construct.
    PrefabImporter(Context* context);
    virtual ~PrefabImporter();

    virtual void SetDefaults();

    bool Import(const String& guid);

protected:

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

};

}
