
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>
#include "AssetDatabase.h"
#include "AssetImporter.h"

namespace ToolCore
{

AssetImporter::AssetImporter(Context* context) : Object(context),
    dirty_(true),
    timestamp_(0)
{
    SetDefaults();
}

AssetImporter::~AssetImporter()
{

}

void AssetImporter::SetDefaults()
{

}

bool AssetImporter::Load(const String& guid)
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);
    assert(asset);

    const String& path = asset->GetPath();
    String assetPath = path + ".asset";

    SharedPtr<File> file(new File(context_, assetPath));

    json_ = new JSONFile(context_);
    json_->Load(*file);
    file->Close();

    LoadInternal();

    json_ = 0;


    return true;
}

bool AssetImporter::LoadInternal()
{
    JSONValue root = json_->GetRoot();

    assert(root.GetInt("version") == 1);
    guid_ = root.GetString("guid");
    timestamp_ = (unsigned) root.GetDouble("timestamp");

    return true;
}

bool AssetImporter::Save(const String& guid)
{

    FileSystem* fs = GetSubsystem<FileSystem>();
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    Asset* asset = db->GetAssetByGUID(guid);
    assert(asset);

    const String& path = asset->GetPath();
    String assetPath = path + ".asset";

    timestamp_ = fs->GetLastModifiedTime(path);
    guid_ = guid;

    json_ = new JSONFile(context_);
    json_->CreateRoot();
    SaveInternal();

    SharedPtr<File> file(new File(context_, assetPath, FILE_WRITE));
    json_->Save(*file, String("   "));
    file->Close();

    json_ = 0;

    return true;
}

bool AssetImporter::SaveInternal()
{
    JSONValue root = json_->GetRoot();

    root.SetInt("version", ASSET_VERSION);
    root.SetString("guid", guid_);
    root.SetDouble("timestamp", (double) timestamp_);

    return true;
}


}
