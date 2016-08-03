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
#include <Atomic/Atomic2D/AnimationSet2D.h>
#include <Atomic/Atomic2D/AnimatedSprite2D.h>

#include "Asset.h"
#include "AssetDatabase.h"
#include "SpriterImporter.h"

namespace ToolCore
{

SpriterImporter::SpriterImporter(Context* context, Asset *asset) : AssetImporter(context, asset)
{
    requiresCacheFile_ = false;
}

SpriterImporter::~SpriterImporter()
{

}

void SpriterImporter::SetDefaults()
{
    AssetImporter::SetDefaults();
}

bool SpriterImporter::Import()
{
    return true;
}

bool SpriterImporter::LoadSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::LoadSettingsInternal(jsonRoot))
        return false;

    JSONValue import = jsonRoot.Get("SpriterImporter");

    return true;
}

bool SpriterImporter::SaveSettingsInternal(JSONValue& jsonRoot)
{
    if (!AssetImporter::SaveSettingsInternal(jsonRoot))
        return false;

    JSONValue import(JSONValue::emptyObject);
    jsonRoot.Set("SpriterImporter", import);

    return true;
}

Resource* SpriterImporter::GetResource(const String& typeName)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    AnimationSet2D* animSet = cache->GetResource<AnimationSet2D>(asset_->GetPath());

    return animSet;

}

Node* SpriterImporter::InstantiateNode(Node* parent, const String& name)
{

    AnimationSet2D* animationSet = (AnimationSet2D*) GetResource();

    if (!animationSet)
        return 0;

    String animationName;

    if (animationSet->GetNumAnimations())
    {
        animationName = animationSet->GetAnimation(0);
    }

    Node* node = parent->CreateChild(name);

    AnimatedSprite2D* sprite = node->CreateComponent<AnimatedSprite2D>();

    if (!animationName.Length())
        sprite->SetAnimationSet(animationSet);
    else
        sprite->SetAnimation(animationName);

    return node;

}

}
