#include <ThirdParty/CEF/include/cef_render_handler.h>

#include <Atomic/IO/Log.h>

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Technique.h>

#include "WebClient.h"
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
        rect = CefRect(0, 0, webTexture2D_->GetWidth(), webTexture2D_->GetHeight());
        return true;
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                 const void *buffer, int width, int height) OVERRIDE
    {

        if (type == PET_VIEW)
        {
            if (width == webTexture2D_->GetWidth() && height == webTexture2D_->GetHeight())
            {
                Texture2D* tex = webTexture2D_->texture_;
                tex->SetData(0, 0, 0, width, height, buffer);
            }
        }

    }

private:

    WeakPtr<WebTexture2D> webTexture2D_;

};

WebTexture2D::WebTexture2D(Context* context) : WebRenderHandler(context)
{
    d_ = new WebTexture2DPrivate(this);
    d_->AddRef();

    texture_ = new Texture2D(context_);
    texture_->SetNumLevels(1);
    texture_->SetFilterMode(FILTER_BILINEAR);

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    material_ = new Material(context_);
    material_->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
    material_->SetTexture(TU_DIFFUSE, texture_);
}

WebTexture2D::~WebTexture2D()
{
    //d_->Release();
}

CefRenderHandler* WebTexture2D::GetCEFRenderHandler()
{
    return d_;
}

int WebTexture2D::GetWidth() const
{
    return texture_->GetWidth();
}

int WebTexture2D::GetHeight() const
{
    return texture_->GetHeight();
}

void WebTexture2D::SetSize(int width, int height)
{
    if (width == texture_->GetWidth() && height == texture_->GetHeight())
        return;

    texture_->SetSize(width, height, Graphics::GetBGRAFormat(), TEXTURE_DYNAMIC);

}

}
