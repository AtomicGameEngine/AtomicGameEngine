
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Container/List.h>
#include "Asset.h"

using namespace Atomic;

namespace ToolCore
{

class Project;

class AssetDatabase : public Object
{
    OBJECT(AssetDatabase);

public:
    /// Construct.
    AssetDatabase(Context* context);
    virtual ~AssetDatabase();

    Asset* GetAssetByGUID(const String& guid);
    Asset* GetAssetByPath(const String& guid);

    String GetCachePath();

    void Scan();

    void GetFolderAssets(String folder, PODVector<Asset*>& assets) const;

    void GetDirtyAssets(PODVector<Asset*>& assets);

private:

    void HandleProjectLoaded(StringHash eventType, VariantMap& eventData);

    String GeneratePathGUID(const String& path);
    void Import(const String& path);

    SharedPtr<Project> project_;
    List<SharedPtr<Asset>> assets_;

};

}
