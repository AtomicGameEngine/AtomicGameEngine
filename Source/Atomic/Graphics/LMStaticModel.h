// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "../Graphics/StaticModel.h"
#include "../Graphics/Texture2D.h"
#include "../Graphics/Material.h"

namespace Atomic
{

class LMStaticModel: public StaticModel
{
    ATOMIC_OBJECT(LMStaticModel, StaticModel);

public:
    /// Construct.
    LMStaticModel(Context* context);
    /// Destruct.
    virtual ~LMStaticModel();

    /// Register object factory. StaticModel must be registered first.
    static void RegisterObject(Context* context);

    virtual void UpdateBatches(const FrameInfo& frame);

    Vector4 lightmapTilingOffset_;

    void SetLightmapTextureAttr(const ResourceRef& value);
    ResourceRef GetLightmapTextureAttr() const;

    void SetLightmapTexure(Texture2D* texture)
    {
        lightmapTexture_ = texture;
    }

private:
    SharedPtr<Texture2D> lightmapTexture_;

    SharedPtr<Material> lightmapMaterial_;

};

}
