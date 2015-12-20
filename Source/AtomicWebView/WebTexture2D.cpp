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
    if (currentWidth_ == width && currentHeight_ == height)
        return;

    currentWidth_ = width;
    currentHeight_ = height;

    unsigned newMaxWidth = NextPowerOfTwo(width);
    unsigned newMaxHeight = NextPowerOfTwo(height);

    SetMaxSize(newMaxWidth, newMaxHeight);

    if (webClient_.NotNull())
        webClient_->WasResized();

}

void WebTexture2D::SetMaxSize(unsigned width, unsigned height)
{
    if (maxWidth_ == width && height == maxHeight_)
        return;

    if (!IsPowerOfTwo(width) || !IsPowerOfTwo(height))
    {
        LOGERRORF("WebTexture2D::SetMaxSize - attempted to set size to %ux%u (must be a power of two)", width, height );
        return;
    }

    maxWidth_ = width;
    maxHeight_ = height;

    texture_->SetSize(maxWidth_, maxHeight_, Graphics::GetBGRAFormat(), TEXTURE_DYNAMIC);

    LOGDEBUGF("WebTexture2D max size set: %u %u", maxWidth_, maxHeight_);

}

}
