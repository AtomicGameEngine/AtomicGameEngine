
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

bool SpriterImporter::LoadSettingsInternal()
{
    if (!AssetImporter::LoadSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.GetChild("SpriterImporter", JSON_OBJECT);

    return true;
}

bool SpriterImporter::SaveSettingsInternal()
{
    if (!AssetImporter::SaveSettingsInternal())
        return false;

    JSONValue import = jsonRoot_.CreateChild("SpriterImporter");

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
