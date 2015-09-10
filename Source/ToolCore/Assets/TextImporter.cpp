
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

bool TextImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("TextImporter");

    return true;
}

bool TextImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("TextImporter", import);

    return true;
}



}
