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

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/Image.h>
#include <Atomic/Atomic2D/Sprite2D.h>
#include <Atomic/Atomic2D/StaticSprite2D.h>
#include <Atomic/IO/FileSystem.h>

#include <ToolCore/Import/ImportConfig.h>
#include <Atomic/Graphics/Renderer.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "TextureImporter.h"

namespace ToolCore
{

	TextureImporter::TextureImporter(Context* context, Asset *asset) : AssetImporter(context, asset),
		compressTextures_(false), compressedSize_(0)
{
    ApplyProjectImportConfig();
}

TextureImporter::~TextureImporter()
{

}

void TextureImporter::SetDefaults()
{
    AssetImporter::SetDefaults();

	compressedSize_ = 0;

}

bool TextureImporter::Import()
{
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    String cachePath = db->GetCachePath();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    String compressedPath = cachePath + "DDS/" + asset_->GetRelativePath() + ".dds";
    if (fileSystem->FileExists(compressedPath))
        fileSystem->Delete(compressedPath);

    SharedPtr<Image> image = cache->GetTempResource<Image>(asset_->GetPath());

    if (image.Null())
        return false;

    if (compressTextures_ &&
        !image->IsCompressed())
    {
        fileSystem->CreateDirs(cachePath, "DDS/" + Atomic::GetPath(asset_->GetRelativePath()));

		float resizefactor;
		float width = image->GetWidth();
		float height = image->GetHeight();

		if (width > compressedSize_ || height > compressedSize_)
		{
			if (width >= height)
			{
				resizefactor = compressedSize_ / width;
			}
			else
			{
				resizefactor = compressedSize_ / height;
			}

			image->Resize(width*resizefactor, height*resizefactor);
		}

		if (image->SaveDDS(compressedPath))
		{			
			Renderer * renderer = GetSubsystem<Renderer>();
			renderer->ReloadTextures();
		}
    }

    // todo, proper proportions
    image->Resize(64, 64);

    // not sure entirely what we want to do here, though if the cache file doesn't exist
    // will reimport
    image->SavePNG(cachePath + asset_->GetGUID());

    // save thumbnail
    image->SavePNG(cachePath + asset_->GetGUID() + "_thumbnail.png");

    return true;
}

void TextureImporter::ApplyProjectImportConfig()
{
    if (ImportConfig::IsLoaded())
    {
        VariantMap tiParameters;
        ImportConfig::ApplyConfig(tiParameters);
        VariantMap::ConstIterator itr = tiParameters.Begin();

        for (; itr != tiParameters.End(); itr++)
        {
            if (itr->first_ == "tiProcess_CompressTextures")
                compressTextures_ = itr->second_.GetBool();
        }
    }
}

bool TextureImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("TextureImporter");

	SetDefaults();

	if (import.Get("compressionSize").IsNumber())
		compressedSize_ = (CompressedFormat)import.Get("compressionSize").GetInt();

    return true;
}

bool TextureImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
	import.Set("compressionSize", compressedSize_);

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
