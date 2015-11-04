//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
        animationName = animationSet->GetAnimation(0)->GetName();
    }

    Node* node = parent->CreateChild(name);

    AnimatedSprite2D* sprite = node->CreateComponent<AnimatedSprite2D>();

    if (!animationName.Length())
        sprite->SetAnimationSet(animationSet);
    else
        sprite->SetAnimation(animationSet, animationName);

    return node;

}

}
