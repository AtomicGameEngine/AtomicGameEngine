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

    ATOMIC_OBJECT(WebTexture2D, WebRenderHandler)

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
