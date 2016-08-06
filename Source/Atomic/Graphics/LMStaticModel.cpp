// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "Precompiled.h"
#include "../Core/Context.h"
#include "../Resource/ResourceCache.h"
#include "../Graphics/Technique.h"
#include "LMStaticModel.h"

namespace Atomic
{

extern const char* GEOMETRY_CATEGORY;


LMStaticModel::LMStaticModel(Context* context) :
    StaticModel(context),
    lightmapTilingOffset_(1.0f, 1.0f, 0.0f, 0.0f)
{

}

LMStaticModel::~LMStaticModel()
{

}

void LMStaticModel::RegisterObject(Context* context)
{
    context->RegisterFactory<LMStaticModel>(GEOMETRY_CATEGORY);

    ATOMIC_COPY_BASE_ATTRIBUTES(StaticModel);
    ATOMIC_ATTRIBUTE("Lightmap Tiling Offset", Vector4, lightmapTilingOffset_ , Vector4(1.0f, 1.0f, 0.0f, 0.0f), AM_DEFAULT);
    ATOMIC_MIXED_ACCESSOR_ATTRIBUTE("LightmapTexture", GetLightmapTextureAttr, SetLightmapTextureAttr, ResourceRef, ResourceRef(Texture2D::GetTypeStatic()), AM_DEFAULT);

}

void LMStaticModel::SetLightmapTextureAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    if (value.name_.Length())
    {
        lightmapTexture_ = cache->GetResource<Texture2D>(value.name_);
    }
    else
    {
        lightmapTexture_ = NULL;
    }

    // TODO: This is making a separate material for each model
    // we should be able to factor this into batching
    if (lightmapTexture_.NotNull() && lightmapMaterial_.Null())
    {
        Material* base = GetMaterial(0);
        if (base)
        {
            ResourceCache* cache = GetSubsystem<ResourceCache>();
            lightmapMaterial_ = base->Clone();
            Technique* technique = cache->GetResource<Technique>("Techniques/DiffLightMap.xml");
            lightmapMaterial_->SetTechnique(0, technique);
            lightmapMaterial_->SetTexture(TU_EMISSIVE, lightmapTexture_);
        }
    }


}

ResourceRef LMStaticModel::GetLightmapTextureAttr() const
{
    return GetResourceRef(lightmapTexture_, Texture2D::GetTypeStatic());
}

void LMStaticModel::UpdateBatches(const FrameInfo& frame)
{
    /*
    StaticModel::UpdateBatches(frame);

    if (batches_.Size() > 1)
    {
        for (unsigned i = 0; i < batches_.Size(); ++i)
        {
            batches_[i].lightmapTilingOffset_ = &lightmapTilingOffset_;
            batches_[i].geometryType_ = GEOM_STATIC_NOINSTANCING;

            if (!lightmapMaterial_.Null())
                batches_[i].material_ = lightmapMaterial_;
        }
    }
    else if (batches_.Size() == 1)
    {
        batches_[0].lightmapTilingOffset_ = &lightmapTilingOffset_;
        batches_[0].geometryType_ = GEOM_STATIC_NOINSTANCING;
        if (!lightmapMaterial_.Null())
            batches_[0].material_ = lightmapMaterial_;

    }
    */

}


}


