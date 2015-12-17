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

        if (type == PET_VIEW)
        {
            webTexture2D_->texture_->SetData(0, 0, 0, width, height, buffer);
        }


        /*
        if (dirtyRects.size() == 1 && width == )
            return;

        int vwidth = width;
        int vheight = height;

        if (vwidth > webTexture2D_->texture_->GetWidth())
            vwidth = webTexture2D_->texture_->GetWidth();

        if (vheight > webTexture2D_->texture_->GetHeight())
            vheight = webTexture2D_->texture_->GetHeight();


        */
    }

private:

    WeakPtr<WebTexture2D> webTexture2D_;

};

WebTexture2D::WebTexture2D(Context* context, int width, int height) : WebRenderHandler(context)
{
    d_ = new WebTexture2DPrivate(this);
    d_->AddRef();

    texture_ = new Texture2D(context_);
    texture_->SetNumLevels(1);
    texture_->SetFilterMode(FILTER_BILINEAR);

    SetCurrentSize(width, height);

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


void WebTexture2D::SetCurrentSize(unsigned width, unsigned height)
{
    currentWidth_ = width;
    currentHeight_ = height;

    unsigned newMaxWidth = NextPowerOfTwo(width);
    unsigned newMaxHeight = NextPowerOfTwo(height);

    if (newMaxWidth != maxWidth_ || newMaxHeight != maxHeight_)
    {
        SetMaxSize(newMaxWidth, newMaxHeight);
    }

}

void WebTexture2D::SetMaxSize(unsigned width, unsigned height)
{
    maxWidth_ = width;
    maxHeight_ = height;

    texture_->SetSize(maxWidth_, maxHeight_, Graphics::GetBGRAFormat(), TEXTURE_DYNAMIC);

}

}
