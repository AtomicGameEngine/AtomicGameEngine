
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

    virtual void SetCurrentWidth(unsigned width);
    virtual void SetCurrentHeight(unsigned height);

    virtual void SetMaxWidth(unsigned width);
    virtual void SetMaxHeight(unsigned height);

private:

    SharedPtr<Texture2D> texture_;
    SharedPtr<Material> material_;

    WebTexture2DPrivate* d_;

};

}
