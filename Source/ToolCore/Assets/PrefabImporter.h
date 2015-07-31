
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

    virtual bool Preload();

protected:

    bool Import();

    virtual bool LoadSettingsInternal();
    virtual bool SaveSettingsInternal();

private:

    void HandlePrefabSave(StringHash eventType, VariantMap& eventData);

   SharedPtr<Atomic::Scene> preloadResourceScene_;

};

}
