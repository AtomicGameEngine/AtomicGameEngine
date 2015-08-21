
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Audio/Sound.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "TextImporter.h"

namespace ToolCore
{

TextImporter::TextImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{

}

TextImporter::~TextImporter()
{

}

void TextImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool TextImporter::Import()
{
    return true;
}

bool TextImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("TextImporter", JSON_OBJECT);

    return true;
}

bool TextImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("TextImporter");

    return true;
}



}
