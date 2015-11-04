//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>
#include <Atomic/Atomic2D/Sprite2D.h>
#include <Atomic/Atomic2D/StaticSprite2D.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "TextureImporter.h"

namespace ToolCore
{

TextureImporter::TextureImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{

}

TextureImporter::~TextureImporter()
{

}

void TextureImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool TextureImporter::Import()
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SharedPtr<Image> image = cache->GetTempResource<Image>(asset_->GetPath());

    if (image.Null())
        return false;

    // todo, proper proportions
    image->Resize(64, 64);

    String cachePath = db->GetCachePath();

    // not sure entirely what we want to do here, though if the cache file doesn't exist
    // will reimport
    image->SavePNG(cachePath + asset_->GetGUID());

    // save thumbnail
    image->SavePNG(cachePath + asset_->GetGUID() + "_thumbnail.png");

    return true;
}

bool TextureImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("TextureImporter");

    return true;
}

bool TextureImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("TextureImporter", import);

    return true;
}

Resource* TextureImporter::GetResource(const String& typeName)
{
    if (!typeName.Length())
        return 0;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    return cache->GetResource(typeName, asset_->GetPath());

}

Node* TextureImporter::InstantiateNode(Node* parent, const String& name)
{

    Node* node = parent->CreateChild(name);

    Sprite2D* spriteGraphic = GetSubsystem<ResourceCache>()->GetResource<Sprite2D>(asset_->GetPath());

    StaticSprite2D* sprite = node->CreateComponent<StaticSprite2D>();

    sprite->SetSprite(spriteGraphic);

    return node;

}



}
