
#include <Poco/MD5Engine.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Math/Random.h>

#include <Atomic/Resource/ResourceCache.h>

#include "../ToolEvents.h"
#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "AssetEvents.h"
#include "AssetDatabase.h"


namespace ToolCore
{

AssetDatabase::AssetDatabase(Context* context) : Object(context)
{
    SubscribeToEvent(E_PROJECTLOADED, HANDLER(AssetDatabase, HandleProjectLoaded));
}

AssetDatabase::~AssetDatabase()
{

}

String AssetDatabase::GetCachePath()
{
    if (project_.Null())
        return String::EMPTY;

    return project_->GetProjectPath() + "Cache/";

}

String AssetDatabase::GenerateAssetGUID()
{

    Time* time = GetSubsystem<Time>();

    while (true)
    {
        Poco::MD5Engine md5;
        PODVector<unsigned> data;

        for (unsigned i = 0; i < 16; i++)
        {
            data.Push(time->GetTimeSinceEpoch() + Rand());
        }

        md5.update(&data[0], data.Size() * sizeof(unsigned));

        String guid = Poco::MD5Engine::digestToHex(md5.digest()).c_str();

        if (!usedGUID_.Contains(guid))
        {
            RegisterGUID(guid);
            return guid;
        }
    }

    assert(0);
    return "";
}

void AssetDatabase::RegisterGUID(const String& guid)
{
    if (usedGUID_.Contains(guid))
    {
        assert(0);
    }

    usedGUID_.Push(guid);
}

void AssetDatabase::Import(const String& path)
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    // nothing for now
    if (fs->DirExists(path))
        return;
}

Asset* AssetDatabase::GetAssetByGUID(const String& guid)
{
    List<SharedPtr<Asset>>::ConstIterator itr = assets_.Begin();

    while (itr != assets_.End())
    {
        if (guid == (*itr)->GetGUID())
            return *itr;

        itr++;
    }

    return 0;

}

Asset* AssetDatabase::GetAssetByPath(const String& path)
{
    List<SharedPtr<Asset>>::ConstIterator itr = assets_.Begin();

    while (itr != assets_.End())
    {
        if (path == (*itr)->GetPath())
            return *itr;

        itr++;
    }

    return 0;

}

void AssetDatabase::PruneOrphanedDotAssetFiles()
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    const String& resourcePath = project_->GetResourcePath();

    Vector<String> allResults;

    fs->ScanDir(allResults, resourcePath, "*.asset", SCAN_FILES, true);

    for (unsigned i = 0; i < allResults.Size(); i++)
    {
        String dotAssetFilename = resourcePath + allResults[i];
        String assetFilename = ReplaceExtension(dotAssetFilename, "");

        // remove orphaned asset files
        if (!fs->FileExists(assetFilename) && !fs->DirExists(assetFilename))
        {

            LOGINFOF("Removing orphaned asset file: %s", dotAssetFilename.CString());
            fs->Delete(dotAssetFilename);
        }

    }
}

String AssetDatabase::GetDotAssetFilename(const String& path)
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    String assetFilename = path + ".asset";

    if (fs->DirExists(path)) {

        assetFilename = RemoveTrailingSlash(path) + ".asset";
    }

    return assetFilename;

}

void AssetDatabase::AddAsset(SharedPtr<Asset>& asset)
{

    assert(asset->GetGUID().Length());

    assets_.Push(asset);

    VariantMap eventData;
    eventData[ResourceAdded::P_GUID] = asset->GetGUID();
    SendEvent(E_RESOURCEADDED, eventData);
}

void AssetDatabase::ImportDirtyAssets()
{

    PODVector<Asset*> assets;
    GetDirtyAssets(assets);

    for (unsigned i = 0; i < assets.Size(); i++)
    {
        assets[i]->Import();
        assets[i]->Save();
    }

}

void AssetDatabase::PreloadAssets()
{

    List<SharedPtr<Asset>>::ConstIterator itr = assets_.Begin();

    while (itr != assets_.End())
    {
        (*itr)->Preload();
        itr++;
    }

}


void AssetDatabase::Scan()
{
    PruneOrphanedDotAssetFiles();

    FileSystem* fs = GetSubsystem<FileSystem>();
    const String& resourcePath = project_->GetResourcePath();

    Vector<String> allResults;

    fs->ScanDir(allResults, resourcePath, "", SCAN_FILES | SCAN_DIRS, true);

    Vector<String> filteredResults;

    for (unsigned i = 0; i < allResults.Size(); i++)
    {
        allResults[i] = resourcePath + allResults[i];

        const String& path = allResults[i];

        if (path.StartsWith(".") || path.EndsWith("."))
            continue;

        String ext = GetExtension(path);

        if (ext == ".asset")
            continue;

        filteredResults.Push(path);
    }

    for (unsigned i = 0; i < filteredResults.Size(); i++)
    {
        const String& path = filteredResults[i];

        String dotAssetFilename = GetDotAssetFilename(path);

        if (!fs->FileExists(dotAssetFilename))
        {
            // new asset
            SharedPtr<Asset> asset(new Asset(context_));

            if (asset->SetPath(path))
                AddAsset(asset);
        }
        else
        {
            SharedPtr<File> file(new File(context_, dotAssetFilename));
            SharedPtr<JSONFile> json(new JSONFile(context_));
            json->Load(*file);
            file->Close();

            JSONValue root = json->GetRoot();

            assert(root.GetInt("version") == ASSET_VERSION);

            String guid = root.GetString("guid");

            if (!GetAssetByGUID(guid))
            {
                SharedPtr<Asset> asset(new Asset(context_));
                asset->SetPath(path);
                AddAsset(asset);
            }

        }

    }

    PreloadAssets();

    ImportDirtyAssets();

}

void AssetDatabase::GetFolderAssets(String folder, PODVector<Asset*>& assets) const
{
    if (project_.Null())
        return;

    List<SharedPtr<Asset>>::ConstIterator itr = assets_.Begin();

    if (!folder.Length())
    {
        folder = project_->GetResourcePath();
    }

    folder = AddTrailingSlash(folder);

    while (itr != assets_.End())
    {
        String path = GetPath((*itr)->GetPath());

        if (path == folder)
            assets.Push(*itr);

        itr++;
    }

}

void AssetDatabase::GetAssetsByImporterType(StringHash type, PODVector<Asset*>& assets) const
{
    assets.Clear();

    List<SharedPtr<Asset>>::ConstIterator itr = assets_.Begin();

    while (itr != assets_.End())
    {
        Asset* asset = *itr;

        if (asset->GetImporterType() == type)
            assets.Push(asset);

        itr++;
    }

}

void AssetDatabase::GetDirtyAssets(PODVector<Asset*>& assets)
{
    assets.Clear();

    List<SharedPtr<Asset>>::ConstIterator itr = assets_.Begin();

    while (itr != assets_.End())
    {
        if ((*itr)->IsDirty())
            assets.Push(*itr);
        itr++;
    }
}

void AssetDatabase::HandleProjectLoaded(StringHash eventType, VariantMap& eventData)
{
    project_ = GetSubsystem<ToolSystem>()->GetProject();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    cache->AddResourceDir(GetCachePath());

    Scan();
}

}
