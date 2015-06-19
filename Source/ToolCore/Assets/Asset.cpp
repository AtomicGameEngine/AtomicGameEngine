
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include "AssetDatabase.h"
#include "ModelImporter.h"
#include "FolderImporter.h"
#include "SceneImporter.h"
#include "MaterialImporter.h"
#include "Asset.h"

namespace ToolCore
{

Asset::Asset(Context* context) :
    Object(context),
    dirty_(false),
    isFolder_(false)
{

}

Asset::~Asset()
{

}

bool Asset::CheckCacheFile()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();

    String cacheFile = cachePath + guid_;

    if (!fs->FileExists(cacheFile))
        return false;

    return true;
}

bool Asset::Import()
{
    if (importer_.Null())
        return true;

    return importer_->Import(guid_);
}

// load .asset
bool Asset::Load()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    AssetDatabase* db = GetSubsystem<AssetDatabase>();

    String assetFilename = GetDotAssetFilename();

    SharedPtr<File> file(new File(context_, assetFilename));
    json_ = new JSONFile(context_);
    json_->Load(*file);
    file->Close();

    JSONValue root = json_->GetRoot();

    assert(root.GetInt("version") == ASSET_VERSION);

    guid_ = root.GetString("guid");

    db->RegisterGUID(guid_);

    timestamp_ = (unsigned) root.GetDouble("timestamp");

    dirty_ = false;
    if (!CheckCacheFile() || timestamp_ < fs->GetLastModifiedTime(path_))
        dirty_ = true;

    // handle import

    if (importer_.NotNull())
        importer_->LoadSettings(root);

    json_ = 0;

    return true;

}

// save .asset
bool Asset::Save()
{
    String assetFilename = GetDotAssetFilename();

    json_ = new JSONFile(context_);

    JSONValue root = json_->CreateRoot();

    root.SetInt("version", ASSET_VERSION);
    root.SetString("guid", guid_);
    root.SetDouble("timestamp", (double) timestamp_);

    // handle import

    if (importer_.NotNull())
    {
        importer_->SaveSettings(root);

        SharedPtr<File> file(new File(context_, assetFilename, FILE_WRITE));
        json_->Save(*file);
        file->Close();
    }

    json_ = 0;

    return true;

}

String Asset::GetDotAssetFilename()
{
    assert(path_.Length());

    FileSystem* fs = GetSubsystem<FileSystem>();

    String assetFilename = path_ + ".asset";

    if (fs->DirExists(path_)) {

        assetFilename = RemoveTrailingSlash(path_) + ".asset";

    }

    return assetFilename;

}

bool Asset::CreateImporter()
{
    assert(importer_.Null());

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->DirExists(path_))
    {
        name_ = GetFileName(RemoveTrailingSlash(path_));
        isFolder_ = true;
        importer_ = new FolderImporter(context_);
    }
    else
    {
        String ext = GetExtension(path_);

        name_ = GetFileName(path_);

        // todo, externalize recognizers
        if (ext == ".fbx")
        {
            importer_ = new ModelImporter(context_);
        }
        else if (ext == ".scene")
        {
            importer_ = new SceneImporter(context_);
        }
        else if (ext == ".material")
        {
            importer_ = new MaterialImporter(context_);
        }

    }

    if (importer_.Null())
        return false;

    return true;

}


bool Asset::SetPath(const String& path)
{

    assert(!guid_.Length());
    assert(!path_.Length());

    // need to update path, not set, which should only be done on first import
    assert(importer_.Null());

    FileSystem* fs = GetSubsystem<FileSystem>();
    AssetDatabase* db = GetSubsystem<AssetDatabase>();

    path_ = path;

    // create importer based on path
    CreateImporter();

    String assetFilename = GetDotAssetFilename();

    if (fs->FileExists(assetFilename))
    {
        // load the json, todo: handle fail
        Load();
    }
    else
    {
        guid_ = db->GenerateAssetGUID();
        timestamp_ = fs->GetLastModifiedTime(path);

        Save();
    }

    // TODO: handle failed



}

}
