
#pragma once

#include <Atomic/Graphics/Texture2D.h>
#include <Atomic/Graphics/Material.h>

#include "WebRenderHandler.h"

namespace Atomic
{

class WebTexture2DPrivate;


/// A render handler which uses a Texture2D and can be used for UI, 2D, and 3D rendering
class ATOMIC_API WebTexture2D : public WebRenderHandler
{
    friend class WebTexture2DPrivate;

    OBJECT(WebTexture2D)

public:

    /// Construct.
    WebTexture2D(Context* context);
    /// Destruct.
    virtual ~WebTexture2D();

    /// Get the current width of the texture
    int GetWidth() const;    
    /// Get the current height of the texture
    int GetHeight() const;
    /// Get the Texture2D associated with the WebTexture2D
    Texture2D* GetTexture2D() const { return texture_; }
    /// get the clear color for the WebTexture
    const Color& GetClearColor() const { return clearColor_; }


    /// Set the dimensions of the texture
    void SetSize(int width, int height);

    /// Set the clear color for the WebTexture
    void SetClearColor(const Color& color) { clearColor_ = color; }

    /// Get the (internal) CEFRenderHandler
    CefRenderHandler* GetCEFRenderHandler();

private:

    Color clearColor_;

    SharedPtr<Texture2D> texture_;

    WebTexture2DPrivate* d_;

};

}
