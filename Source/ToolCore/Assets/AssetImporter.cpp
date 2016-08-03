//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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

        ATOMIC_LOGERRORF("Unable to rename asset: %s to %s", GetNativePath(oldPath + ".asset").CString(), GetNativePath(newPath + ".asset").CString());
        return false;
    }

    // now rename the asset file itself
    if (!fs->Rename(oldPath, newPath))
    {
        asset_->name_ = oldName;
        asset_->path_ = oldPath;

        // restore .asset
        fs->Rename(newPath + ".asset", oldPath + ".asset");

        ATOMIC_LOGERRORF("Unable to rename: %s to %s", GetNativePath(oldPath).CString(), GetNativePath(newPath).CString());
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
