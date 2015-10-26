//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include "AssetDatabase.h"
#include "AssetImporter.h"

namespace ToolCore
{

AssetImporter::AssetImporter(Context* context, Asset *asset) : Object(context),
    asset_(asset),
    requiresCacheFile_(true)
{
    SetDefaults();
}

AssetImporter::~AssetImporter()
{

}

void AssetImporter::SetDefaults()
{

}

bool AssetImporter::LoadSettings(JSONValue& root)
{
    LoadSettingsInternal(root);
    return true;
}

bool AssetImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    return true;
}

bool AssetImporter::SaveSettings(JSONValue& root)
{
    SaveSettingsInternal(root);
    return true;
}

bool AssetImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    return true;
}

bool AssetImporter::Move(const String& newPath)
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    if (newPath == asset_->path_)
        return false;

    String oldPath = asset_->path_;
    String oldName = asset_->name_;

    String pathName, newName, ext;

    SplitPath(newPath, pathName, newName, ext);

    // rename asset first, ahead of the filesystem watcher, so the assetdatabase doesn't see a new asset
    asset_->name_ = newName;
    asset_->path_ = newPath;

    // first rename the .asset file
    if (!fs->Rename(oldPath + ".asset", newPath + ".asset"))
    {
        asset_->name_ = oldName;
        asset_->path_ = oldPath;

        LOGERRORF("Unable to rename asset: %s to %s", GetNativePath(oldPath + ".asset").CString(), GetNativePath(newPath + ".asset").CString());
        return false;
    }

    // now rename the asset file itself
    if (!fs->Rename(oldPath, newPath))
    {
        asset_->name_ = oldName;
        asset_->path_ = oldPath;

        // restore .asset
        fs->Rename(newPath + ".asset", oldPath + ".asset");

        LOGERRORF("Unable to rename: %s to %s", GetNativePath(oldPath).CString(), GetNativePath(newPath).CString());
        return false;
    }

    return true;
}

bool AssetImporter::Rename(const String& newName)
{
    String pathName, fileName, ext;

    SplitPath(asset_->path_, pathName, fileName, ext);

    String newPath = pathName + newName + ext;

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->FileExists(newPath) || fs->DirExists(newPath))
        return false;

    return Move(newPath);

}


}
