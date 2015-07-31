
#pragma once

#include <Atomic/Core/Object.h>

#include "AssetImporter.h"

using namespace Atomic;

namespace ToolCore
{

#define ASSET_VERSION 1

class Asset : public Object
{
    friend class AssetDatabase;

    OBJECT(Asset);

public:
    /// Construct.
    Asset(Context* context);
    virtual ~Asset();

    bool Import();
    bool Preload();

    // the .fbx, .png, etc path, attempts to load .asset, creates missing .asset
    bool SetPath(const String& path);

    const String& GetGUID() const { return guid_; }
    const String& GetName() const { return name_; }
    const String& GetPath() const { return path_; }
    /// Get the path relative to project
    String GetRelativePath();
    String GetCachePath() const;

    const StringHash GetImporterType() { return importer_.Null() ? String::EMPTY : importer_->GetType(); }
    const String& GetImporterTypeName() { return importer_.Null() ? String::EMPTY : importer_->GetTypeName(); }

    AssetImporter* GetImporter() { return importer_; }

    void PostImportError(const String& message);

    Asset* GetParent();

    void SetDirty(bool dirty) { dirty_ = dirty; }
    bool IsDirty() const { return dirty_; }

    /// Get the last timestamp as seen by the AssetDatabase
    unsigned GetFileTimestamp() { return fileTimestamp_; }

    /// Sets the time stamp to the asset files current time
    void UpdateFileTimestamp();

    // get the .asset filename
    String GetDotAssetFilename();

    bool IsFolder() const { return isFolder_; }

    // load .asset
    bool Load();
    // save .asset
    bool Save();


private:

    bool CreateImporter();

    bool CheckCacheFile();

    String guid_;

    // can change
    String path_;
    String name_;

    bool dirty_;
    bool isFolder_;

    // the current timestamp of the asset as seen by the asset database
    // used to catch when the asset needs to be marked dirty when notified
    // that the file has changed (the resource system will send a changed file
    // event when the resource is first added)
    unsigned fileTimestamp_;

    SharedPtr<JSONFile> json_;
    SharedPtr<AssetImporter> importer_;
};

}
