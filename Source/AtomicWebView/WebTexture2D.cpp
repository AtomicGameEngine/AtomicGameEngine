//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifdef ATOMIC_PLATFORM_OSX
#include <ThirdParty/GLEW/glew.h>
#endif

#include <include/cef_render_handler.h>

#include <Atomic/Math/Rect.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/GraphicsImpl.h>
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

#ifdef ATOMIC_D3D11
        stagingTexture_ = 0;
        stagingWidth_ = 0;
        stagingHeight_ = 0;
#endif
        webTexture2D_ = webTexture2D;
        graphics_ = webTexture2D->GetSubsystem<Graphics>();
        ClearPopupRects();
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

#ifndef ATOMIC_PLATFORM_WINDOWS

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                 const void *buffer, int width, int height) OVERRIDE
    {
        glEnable(GL_TEXTURE_2D);

        if (type == PET_VIEW)
        {
            glBindTexture(GL_TEXTURE_2D, (GLuint) webTexture2D_->GetTexture2D()->GetGPUObjectName());

            glPixelStorei(GL_UNPACK_ROW_LENGTH, width);

            if (dirtyRects.size() == 1 &&
                    dirtyRects[0] == CefRect(0, 0, webTexture2D_->GetWidth(), webTexture2D_->GetHeight()))
            {
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
                glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buffer);
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

            glBindTexture(GL_TEXTURE_2D, (GLuint) webTexture2D_->GetTexture2D()->GetGPUObjectName());

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

#ifndef ATOMIC_OPENGL

    // Windows D3D blitting

#ifdef ATOMIC_D3D11

    void ReleaseStagingTexture()
    {
        if (stagingTexture_)
        {
            stagingTexture_->Release();
        }

        stagingTexture_ = 0;
        stagingWidth_ = 0;
        stagingHeight_ = 0;
    }

    void UpdateStagingTexture()
    {
        if (stagingTexture_)
        {
            ReleaseStagingTexture();
        }

        D3D11_TEXTURE2D_DESC textureDesc;
        memset(&textureDesc, 0, sizeof textureDesc);

        textureDesc.Width = (UINT)webTexture2D_->GetWidth();
        textureDesc.Height = (UINT)webTexture2D_->GetHeight();
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;

        HRESULT hr = graphics_->GetImpl()->GetDevice()->CreateTexture2D(&textureDesc, nullptr, &stagingTexture_);

        if (!SUCCEEDED(hr))
        {
            stagingTexture_ = 0;
            return;
        }

        stagingWidth_ = webTexture2D_->GetWidth();
        stagingHeight_ = webTexture2D_->GetHeight();

    }


#endif


    void D3DBlit(const IntRect& dstRect, unsigned char* src, unsigned srcStride, bool discard = false)
    {

#ifdef ATOMIC_D3D11

        if (!webTexture2D_ || !webTexture2D_->GetWidth() || !webTexture2D_->GetHeight() || !dstRect.Width() || !dstRect.Height())
        {
            return;
        }

        if (!stagingTexture_ || (stagingWidth_ != webTexture2D_->GetWidth()) || (stagingHeight_ != webTexture2D_->GetHeight()))        
        {
            return;
        }
                
        D3D11_BOX box;
        box.left = dstRect.left_;
        box.right = dstRect.right_;
        box.top = dstRect.top_;
        box.bottom = dstRect.bottom_;
        box.front = 0;
        box.back = 1;

        if ((dstRect.left_ + dstRect.Width() > stagingWidth_) || (dstRect.top_ + dstRect.Height() > stagingHeight_))
            return;

        graphics_->GetImpl()->GetDeviceContext()->UpdateSubresource(stagingTexture_, 0, &box, src, srcStride, 0);

        graphics_->GetImpl()->GetDeviceContext()->CopyResource((ID3D11Resource*)webTexture2D_->GetTexture2D()->GetGPUObject(), stagingTexture_);            


#else
        RECT d3dRect;

        d3dRect.left = dstRect.left_;
        d3dRect.top = dstRect.top_;
        d3dRect.right = dstRect.right_;
        d3dRect.bottom = dstRect.bottom_;

        int level = 0;
        DWORD flags = discard ? D3DLOCK_DISCARD : 0;

        D3DLOCKED_RECT d3dLockedRect;
        IDirect3DTexture9* object = (IDirect3DTexture9*) webTexture2D_->GetTexture2D()->GetGPUObject();

        if (!object || FAILED(object->LockRect(level, &d3dLockedRect, (flags & D3DLOCK_DISCARD) ? 0 : &d3dRect, flags)))
        {
            ATOMIC_LOGERROR("WebTexture2D - Could not lock texture");
            return;
        }

        int width = dstRect.Width();
        int height = dstRect.Height();

        for (int j = 0; j < height; ++j)
        {
            unsigned char* dst = (unsigned char*) d3dLockedRect.pBits + j * d3dLockedRect.Pitch;
            memcpy(dst, src, width * 4);
            src += srcStride;
        }

        object->UnlockRect(level);
#endif

    }

    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects,
                 const void *buffer, int width, int height) OVERRIDE
    {

        if (type == PET_VIEW)
        {
            if (dirtyRects.size() == 1 &&
                    dirtyRects[0] == CefRect(0, 0, webTexture2D_->GetWidth(), webTexture2D_->GetHeight()))
            {
                D3DBlit(IntRect(0, 0, width, height), (unsigned char*) buffer, width * 4, true);
                return;
            }

            // Update just the dirty rectangles
            CefRenderHandler::RectList::const_iterator i = dirtyRects.begin();

            for (; i != dirtyRects.end(); ++i)
            {
                const CefRect& rect = *i;
                unsigned char* src = (unsigned char*) buffer;
                src += rect.y * (width * 4) + (rect.x * 4);
                D3DBlit(IntRect(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height), src, width * 4, false);
            }

        }
        else if (type == PET_POPUP && popupRect_.width > 0 && popupRect_.height > 0)
        {
            int  x = popupRect_.x;
            int  y = popupRect_.y;
            int w = width;
            int h = height;
            int viewwidth = webTexture2D_->GetWidth();
            int viewheight = webTexture2D_->GetHeight();

            // Adjust the popup to fit inside the view.
            if (x < 0)
            {
                x = 0;
            }
            if (y < 0)
            {
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

            unsigned char* src = (unsigned char*) buffer;
            D3DBlit(IntRect(x, y, x + w, y + h), src, width * 4, false);

        }

#endif

    }

#endif

private:

#ifdef ATOMIC_D3D11
    ID3D11Texture2D* stagingTexture_;
    int stagingWidth_;
    int stagingHeight_;
#endif

    CefRect popupRect_;
    CefRect popupRectOriginal_;

    WeakPtr<Graphics> graphics_;
    WeakPtr<WebTexture2D> webTexture2D_;

};

WebTexture2D::WebTexture2D(Context* context) : WebRenderHandler(context), clearColor_(Color::WHITE)
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

#ifdef ATOMIC_D3D11
    d_->ReleaseStagingTexture();
#endif
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

    // initialize to white (color should probably be an option)

#ifndef ATOMIC_D3D11
    if (!texture_->SetSize(width, height, Graphics::GetRGBAFormat(), TEXTURE_DYNAMIC))
    {
        ATOMIC_LOGERRORF("Unable to set WebTexture2D size to %i x %i", width, height);
        return;
    }
#else
    
    if (!texture_->SetSize(width, height, DXGI_FORMAT_B8G8R8A8_UNORM, TEXTURE_STATIC))
    {
        ATOMIC_LOGERRORF("Unable to set WebTexture2D size to %i x %i", width, height);
        return;
    }

    d_->UpdateStagingTexture();


#endif

    SharedArrayPtr<unsigned> cleardata(new unsigned[width * height]);
    unsigned color = clearColor_.ToUInt();
    unsigned* ptr = cleardata.Get();
    for (unsigned i = 0; i < width * height; i++, ptr++)
    {
        *ptr = color;
    }

    texture_->SetData(0, 0, 0, width, height, cleardata.Get());

}

}
