
#pragma once

#include <Atomic/Core/Object.h>

#include "AssetImporter.h"

using namespace Atomic;

namespace ToolCore
{

class Asset : public Object
{
    OBJECT(Asset);

public:
    /// Construct.
    Asset(Context* context, const String& guid, unsigned timestamp);
    virtual ~Asset();

    bool Import();

    const String& GetGUID() const { return guid_; }

    const String& GetName() { return name_; }

    const String& GetPath() const { return path_; }
    void SetPath(const String& path);

    void SetDirty(bool dirty) { dirty_ = dirty; }
    bool IsDirty() const { return dirty_; }

    bool IsFolder() const { return isFolder_; }

private:

    String guid_;
    String path_;
    String name_;

    bool dirty_;
    bool isFolder_;

    SharedPtr<AssetImporter> importer_;
};

}
