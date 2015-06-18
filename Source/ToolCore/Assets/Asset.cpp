
#include <Atomic/IO/FileSystem.h>

#include "ModelImporter.h"
#include "FolderImporter.h"
#include "SceneImporter.h"
#include "MaterialImporter.h"
#include "Asset.h"

namespace ToolCore
{

Asset::Asset(Context* context, const String& guid, unsigned timestamp) :
    Object(context),
    guid_(guid),
    dirty_(false),
    isFolder_(false)
{

}

Asset::~Asset()
{

}

bool Asset::Import()
{
    if (importer_.Null())
        return true;

    return importer_->Import(guid_);
}

bool Asset::SetPath(const String& path)
{
    FileSystem* fs = GetSubsystem<FileSystem>();

    path_ = path;

    if (importer_.Null())
    {
        if (fs->DirExists(path))
        {
            name_ = GetFileName(RemoveTrailingSlash(path));
            isFolder_ = true;
            importer_ = new FolderImporter(context_);
        }
        else
        {
            String ext = GetExtension(path);

            // todo, externalize recognizers
            if (ext == ".fbx")
            {
                name_ = GetFileName(path);
                importer_ = new ModelImporter(context_);
            }
            else if (ext == ".scene")
            {
                name_ = GetFileName(path);
                importer_ = new SceneImporter(context_);
            }
            else if (ext == ".material")
            {
                name_ = GetFileName(path);
                importer_ = new MaterialImporter(context_);
            }

        }

        if (importer_.Null())
            return false;

        String assetPath = path + ".asset";

        if (fs->FileExists(assetPath))
        {
            importer_->Load(guid_);
        }
        else
        {
            importer_->Save(guid_);
        }

        SetDirty(importer_->IsDirty());

    }

}

}
