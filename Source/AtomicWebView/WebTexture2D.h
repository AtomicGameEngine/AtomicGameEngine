
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
    WebTexture2D(Context* context);
    /// Destruct.
    virtual ~WebTexture2D();

    int GetWidth() const;
    int GetHeight() const;

    void SetSize(int width, int height);

    CefRenderHandler* GetCEFRenderHandler();

    Texture2D* GetTexture2D() const { return texture_; }

private:

    SharedPtr<Texture2D> texture_;

    WebTexture2DPrivate* d_;

};

}
