
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

    // the .fbx, .png, etc path, attempts to load .asset, creates missing .asset
    bool SetPath(const String& path);

    const String& GetGUID() const { return guid_; }
    const String& GetName() const { return name_; }
    const String& GetPath() const { return path_; }
    String GetCachePath() const;
    unsigned GetTimestamp() const { return timestamp_; }

    const StringHash GetImporterType() { return importer_.Null() ? String::EMPTY : importer_->GetType(); }
    const String& GetImporterTypeName() { return importer_.Null() ? String::EMPTY : importer_->GetTypeName(); }

    AssetImporter* GetImporter() { return importer_; }

    void SetDirty(bool dirty) { dirty_ = dirty; }
    bool IsDirty() const { return dirty_; }

    // get the .asset filename
    String GetDotAssetFilename();

    bool IsFolder() const { return isFolder_; }

private:

    // load .asset
    bool Load();
    // save .asset
    bool Save();

    bool CreateImporter();

    bool CheckCacheFile();

    String guid_;

    // can change
    String path_;
    String name_;
    unsigned timestamp_;

    bool dirty_;
    bool isFolder_;

    SharedPtr<JSONFile> json_;
    SharedPtr<AssetImporter> importer_;
};

}
