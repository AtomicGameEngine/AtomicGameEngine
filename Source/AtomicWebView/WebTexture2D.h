
#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

#include "WebRenderHandler.h"

#pragma once

namespace Atomic
{

class WebTexture2DPrivate;

class ATOMIC_API WebTexture2D : public WebRenderHandler
{
    friend class WebTexture2DPrivate;

    OBJECT(WebTexture2D);

public:

    /// Construct.
    WebTexture2D(Context* context, int width = 1024, int height = 1024);
    /// Destruct.
    virtual ~WebTexture2D();

    virtual void SetCurrentSize(unsigned width, unsigned height);
    virtual void SetMaxSize(unsigned width, unsigned height);

    CefRenderHandler* GetCEFRenderHandler();

    Texture2D* GetTexture2D() const { return texture_; }
    Material* GetMaterial() const { return material_; }

private:

    SharedPtr<Texture2D> texture_;
    SharedPtr<Material> material_;

    WebTexture2DPrivate* d_;

};

}
