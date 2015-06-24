
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include "AssetDatabase.h"
#include "ModelImporter.h"
#include "FolderImporter.h"
#include "SceneImporter.h"
#include "MaterialImporter.h"
#include "TextureImporter.h"
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

    if (!fs->FileExists(cacheFile) || fs->GetLastModifiedTime(cacheFile) < fs->GetLastModifiedTime(path_))
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

    timestamp_ = root.GetUInt("timestamp");

    dirty_ = false;
    if (!CheckCacheFile())
    {
        LOGINFOF("CheckCacheFile:false - %s", path_.CString());
        dirty_ = true;
    }
    else if (timestamp_ < fs->GetLastModifiedTime(path_))
    {
        LOGINFOF("Timestamp:false - %u vs modified %u - %s", timestamp_, fs->GetLastModifiedTime(path_), path_.CString());
        dirty_ = true;
    }


    // handle import

    if (importer_.NotNull())
        importer_->LoadSettings(root);

    json_ = 0;

    return true;

}

// save .asset
bool Asset::Save()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    String assetFilename = GetDotAssetFilename();

    json_ = new JSONFile(context_);

    JSONValue root = json_->CreateRoot();

    root.SetInt("version", ASSET_VERSION);
    root.SetString("guid", guid_);

    // update this where?
    timestamp_ = fs->GetLastModifiedTime(path_);

    root.SetUInt("timestamp", timestamp_);

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

        Vector<String> textureFormats;
        textureFormats.Push(".jpg");
        textureFormats.Push(".png");
        textureFormats.Push(".tga");

        // todo, externalize recognizers
        if (ext == ".fbx")
        {
            importer_ = new ModelImporter(context_, this);
        }
        else if (ext == ".scene")
        {
            importer_ = new SceneImporter(context_);
        }
        else if (ext == ".material")
        {
            importer_ = new MaterialImporter(context_);
        }
        else if (textureFormats.Contains(ext))
        {
            importer_ = new TextureImporter(context_);
        }

    }

    if (importer_.Null())
        return false;

    return true;

}

String Asset::GetCachePath() const
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();
    cachePath += guid_;
    return cachePath;
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
    if (!CreateImporter())
        return false;

    String assetFilename = GetDotAssetFilename();

    if (fs->FileExists(assetFilename))
    {
        // load the json, todo: handle fail
        Load();
    }
    else
    {
        dirty_ = true;
        guid_ = db->GenerateAssetGUID();
        timestamp_ = fs->GetLastModifiedTime(path);

        Save();
    }

    // TODO: handle failed

    return true;

}

}
