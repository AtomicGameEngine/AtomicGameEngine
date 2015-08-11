
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>

#include <AtomicJS/Javascript/JSComponentFile.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "JavascriptImporter.h"


namespace ToolCore
{

JavascriptImporter::JavascriptImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
    isComponentFile_ = false;
}

JavascriptImporter::~JavascriptImporter()
{

}

void JavascriptImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool JavascriptImporter::Import()
{
    isComponentFile_ = false;

    const String& path = asset_->GetPath();

    SharedPtr<File> file(new File(context_, path, FILE_READ));

    unsigned dataSize = file->GetSize();

    SharedArrayPtr<char> buffer(new char[dataSize + 1]);

    if (file->Read(buffer.Get(), dataSize) != dataSize)
        return false;

    buffer[dataSize] = '\0';

    file->Close();

    if (strstr(buffer, "\"atomic component\";"))
        isComponentFile_ = true;

    return true;
}

bool JavascriptImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("JavascriptImporter", JSON_OBJECT);

    isComponentFile_ = import.GetBool("IsComponentFile");

    return true;
}

bool JavascriptImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("JavascriptImporter");

    import.SetBool("IsComponentFile", isComponentFile_);

    return true;
}

Resource* JavascriptImporter::GetResource()
{
    if (!isComponentFile_)
        return 0;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    JSComponentFile* jsc = cache->GetResource<JSComponentFile>(asset_->GetPath());

    return jsc;

}



}
