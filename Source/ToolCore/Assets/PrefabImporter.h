
#pragma once

#include "AssetImporter.h"

namespace Atomic
{
class Scene;
}

namespace ToolCore
{

class PrefabImporter : public AssetImporter
{
    OBJECT(PrefabImporter);

public:
    /// Construct.
    PrefabImporter(Context* context, Asset* asset);
    virtual ~PrefabImporter();

    virtual void SetDefaults();

    bool Import(const String& guid);
    virtual bool Preload();

protected:

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

private:

   SharedPtr<Atomic::Scene> preloadResourceScene_;

};

}
