#include <ThirdParty/CEF/include/cef_render_handler.h>

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Technique.h>

#include "WebTexture2D.h"

namespace Atomic
{

class WebTexture2DPrivate : public CefRenderHandler
{
    friend class WebTexture2D;

public:

    IMPLEMENT_REFCOUNTING(WebTexture2DPrivate)

    WebTexture2DPrivate(WebTexture2D* webTexture2D)
    {
        webTexture2D_ = webTexture2D;
    }

    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE
    {
        rect = CefRect(0, 0, webTexture2D_->GetCurrentWidth(), webTexture2D_->GetCurrentHeight());
        return true;
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                           const void *buffer, int width, int height) OVERRIDE
    {
        webTexture2D_->texture_->SetData(0, 0, 0, width, height, buffer);
    }

private:

    WeakPtr<WebTexture2D> webTexture2D_;

};

WebTexture2D::WebTexture2D(Context* context, int width, int height) : WebRenderHandler(context)
{
    d_ = new WebTexture2DPrivate(this);
    d_->AddRef();

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    currentWidth_ = maxWidth_ = width;
    currentHeight_ = maxHeight_ = height;

    texture_ = new Texture2D(context_);
    texture_->SetNumLevels(1);
    texture_->SetSize(width, height, Graphics::GetBGRAFormat(), TEXTURE_DYNAMIC);
    texture_->SetFilterMode(FILTER_BILINEAR);

    material_ = new Material(context_);

    material_->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
    material_->SetTexture(TU_DIFFUSE, texture_);
}

WebTexture2D::~WebTexture2D()
{
    d_->Release();
}

CefRenderHandler* WebTexture2D::GetCEFRenderHandler()
{
    return d_;
}

void WebTexture2D::SetCurrentWidth(unsigned width)
{

}

void WebTexture2D::SetCurrentHeight(unsigned height)
{

}

void WebTexture2D::SetMaxWidth(unsigned width)
{

}

void WebTexture2D::SetMaxHeight(unsigned height)
{

}

}
