
#ifdef ATOMIC_PLATFORM_OSX
#include <ThirdParty/GLEW/glew.h>
#endif

#include <include/cef_render_handler.h>

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

    void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE
    {
        if (!show)
        {
            // Clear the popup rectangle.
            ClearPopupRects();
        }
    }

    void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE
    {
        if (rect.width <= 0 || rect.height <= 0)
        {
            ClearPopupRects();
            return;
        }

        popupRectOriginal_ = rect;
        popupRect_ = GetPopupRectInWebView(popupRectOriginal_);
    }

    CefRect GetPopupRectInWebView(const CefRect& original_rect) {

        CefRect rc(original_rect);

        // if x or y are negative, move them to 0.
        if (rc.x < 0)
            rc.x = 0;
        if (rc.y < 0)
            rc.y = 0;

        // if popup goes outside the view, try to reposition origin
        if (rc.x + rc.width > webTexture2D_->GetWidth())
            rc.x = webTexture2D_->GetWidth() - rc.width;
        if (rc.y + rc.height > webTexture2D_->GetHeight())
            rc.y = webTexture2D_->GetHeight() - rc.height;

        // if x or y became negative, move them to 0 again.
        if (rc.x < 0)
            rc.x = 0;
        if (rc.y < 0)
            rc.y = 0;

        return rc;
    }

    void ClearPopupRects()
    {
        popupRect_.Set(0, 0, 0, 0);
        popupRectOriginal_.Set(0, 0, 0, 0);
    }

    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) OVERRIDE
    {
        rect = CefRect(0, 0, webTexture2D_->GetWidth(), webTexture2D_->GetHeight());
        return true;
    }

#ifdef ATOMIC_PLATFORM_OSX
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                 const void *buffer, int width, int height) OVERRIDE
    {
        glEnable(GL_TEXTURE_2D);

        if (type == PET_VIEW)
        {
            glBindTexture(GL_TEXTURE_2D, webTexture2D_->GetTexture2D()->GetGPUObject());

            glPixelStorei(GL_UNPACK_ROW_LENGTH, width);

            if (dirtyRects.size() == 1 &&
                    dirtyRects[0] == CefRect(0, 0, webTexture2D_->GetWidth(), webTexture2D_->GetHeight()))
            {
                Texture2D* tex = webTexture2D_->texture_;
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                tex->SetData(0, 0, 0, width, height, buffer);
            }
            else
            {
                // Update just the dirty rectangles.
                CefRenderHandler::RectList::const_iterator i = dirtyRects.begin();

                for (; i != dirtyRects.end(); ++i)
                {
                    const CefRect& rect = *i;

                    glPixelStorei(GL_UNPACK_SKIP_PIXELS, rect.x);
                    glPixelStorei(GL_UNPACK_SKIP_ROWS, rect.y);
                    glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.width,
                                    rect.height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
                                    buffer);

                }

            }

            glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else if (type == PET_POPUP && popupRect_.width > 0 && popupRect_.height > 0)
        {
            int skip_pixels = 0, x = popupRect_.x;
            int skip_rows = 0, y = popupRect_.y;
            int w = width;
            int h = height;
            int viewwidth = webTexture2D_->GetWidth();
            int viewheight = webTexture2D_->GetHeight();

            // Adjust the popup to fit inside the view.
            if (x < 0)
            {
                skip_pixels = -x;
                x = 0;
            }
            if (y < 0)
            {
                skip_rows = -y;
                y = 0;
            }
            if (x + w > viewwidth)
            {
                w -= x + w - viewwidth;
            }
            if (y + h > viewheight)
            {
                h -= y + h - viewheight;
            }

            glBindTexture(GL_TEXTURE_2D, webTexture2D_->GetTexture2D()->GetGPUObject());

            // Update the popup rectangle.
            glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_pixels);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, skip_rows);

            glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_BGRA,
                            GL_UNSIGNED_INT_8_8_8_8_REV, buffer);

            glBindTexture(GL_TEXTURE_2D, 0);

            glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
            glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        }

        glDisable(GL_TEXTURE_2D);

    }
#else

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                 const void *buffer, int width, int height) OVERRIDE
    {
        if (type == PET_VIEW)
        {
            webTexture2D_->GetTexture2D()->SetData(0, 0, 0, width, height, buffer);
        }
    }

#endif

private:

    CefRect popupRect_;
    CefRect popupRectOriginal_;

    WeakPtr<WebTexture2D> webTexture2D_;

};

WebTexture2D::WebTexture2D(Context* context) : WebRenderHandler(context)
{
    d_ = new WebTexture2DPrivate(this);
    d_->AddRef();

    texture_ = new Texture2D(context_);
    texture_->SetNumLevels(1);
    texture_->SetFilterMode(FILTER_NEAREST);
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

    texture_->SetSize(width, height, Graphics::GetRGBAFormat(), TEXTURE_DYNAMIC);

}

}
