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

bool AssetImporter::Rename(const String& newName)
{
    String pathName, fileName, ext;

    SplitPath(asset_->path_, pathName, fileName, ext);

    String newPath = pathName + newName + ext;

    FileSystem* fs = GetSubsystem<FileSystem>();

    if (fs->FileExists(newPath) || fs->DirExists(newPath))
        return false;

    // rename asset first, ahead of the filesystem watcher
    String oldPath = asset_->path_;

    asset_->name_ = newName;
    asset_->path_ = newPath;

    // first rename the .asset file
    if (!fs->Rename(oldPath + ".asset", newPath + ".asset"))
    {
        LOGERRORF("Unable to rename asset: %s to %s", GetNativePath(oldPath + ".asset").CString(), GetNativePath(newPath + ".asset").CString());
        return false;
    }

    // now rename the asset file itself
    if (!fs->Rename(oldPath, newPath))
    {
        // restore .asset
        fs->Rename(newPath + ".asset", oldPath + ".asset");

        LOGERRORF("Unable to rename: %s to %s", GetNativePath(oldPath).CString(), GetNativePath(newPath).CString());
        return false;
    }

    return true;

}


}
