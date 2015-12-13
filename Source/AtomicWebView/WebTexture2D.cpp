
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Technique.h>

#include "WebTexture2D.h"

namespace Atomic
{


WebTexture2D::WebTexture2D(Context* context, int width, int height) : WebRenderer(context)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    texture_ = new Texture2D(context_);
    texture_->SetSize(width, height, Graphics::GetRGBAFormat(), TEXTURE_DYNAMIC);
    texture_->SetFilterMode(FILTER_NEAREST);

    material_ = new Material(context_);

    material_->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
    material_->SetTexture(TU_DIFFUSE, texture_);
}

WebTexture2D::~WebTexture2D()
{

}


}
