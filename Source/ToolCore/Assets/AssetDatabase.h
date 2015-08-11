
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
    Asset* GetAssetByPath(const String& path);
    Asset* GetAssetByCachePath(const String& cachePath);

    String GenerateAssetGUID();
    void RegisterGUID(const String& guid);

    String GetCachePath();

    void DeleteAsset(Asset* asset);

    void Scan();

    void GetFolderAssets(String folder, PODVector<Asset*>& assets) const;

    String GetResourceImporterName(const String& resourceTypeName);

    void GetAssetsByImporterType(StringHash type, PODVector<Asset*>& assets) const;

    void GetDirtyAssets(PODVector<Asset*>& assets);

    String GetDotAssetFilename(const String& path);

private:

    void HandleProjectLoaded(StringHash eventType, VariantMap& eventData);
    void HandleProjectUnloaded(StringHash eventType, VariantMap& eventData);
    void HandleFileChanged(StringHash eventType, VariantMap& eventData);

    void AddAsset(SharedPtr<Asset>& asset);

    void PruneOrphanedDotAssetFiles();

    void Import(const String& path);

    bool ImportDirtyAssets();
    void PreloadAssets();

    SharedPtr<Project> project_;
    List<SharedPtr<Asset>> assets_;

    HashMap<StringHash, String> resourceTypeToImporterType_;

    Vector<String> usedGUID_;

};

}
