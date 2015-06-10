
#include <Poco/MD5Engine.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include "../ToolEvents.h"
#include "../ToolSystem.h"
#include "../Project/Project.h"
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

String AssetDatabase::GeneratePathGUID(const String &path)
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    Poco::MD5Engine md5;

    if (fs->DirExists(path))
    {
        md5.update(path.CString(), path.Length());
    }
    else
    {
        SharedPtr<File> file(new File(context_, path));
        PODVector<unsigned char> data;

        if (!data.Size())
        {
            // zero length file uses path name instead of data
            data.Resize(path.Length());
            memcpy(&data[0], path.CString(), path.Length());
        }
        else
        {
            data.Resize(file->GetSize());
            file->Read(&data[0], data.Size());
        }

        md5.update(&data[0], data.Size());
    }

    return Poco::MD5Engine::digestToHex(md5.digest()).c_str();
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

void AssetDatabase::Scan()
{
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

    Vector<String> importResults;

    for (unsigned i = 0; i < filteredResults.Size(); i++)
    {
        const String& path = filteredResults[i];
        importResults.Push(path);
    }

    for (unsigned i = 0; i < importResults.Size(); i++)
    {
        //Import(importResults[i]);

        const String& path = importResults[i];

        String md5 = GeneratePathGUID(path);

        // get the current time stamp
        unsigned ctimestamp = fs->GetLastModifiedTime(path);

        SharedPtr<Asset> asset(new Asset(context_, md5, ctimestamp));
        assets_.Push(asset);

        asset->SetPath(path);

    }

    PODVector<Asset*> dirty;
    GetDirtyAssets(dirty);

    for (unsigned i = 0; i < dirty.Size(); i++)
    {
        dirty[i]->Import();
    }
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

    Scan();
}

}
