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

class CefRenderHandler;

namespace Atomic
{

class WebClient;


/// Base class for various WebRenderHandlers
class ATOMIC_API WebRenderHandler : public Object
{
    ATOMIC_OBJECT(WebRenderHandler, Object);

public:

    /// Construct.
    WebRenderHandler(Context* context);
    /// Destruct.
    virtual ~WebRenderHandler();

    /// Get the current renderer width
    virtual int GetWidth() const = 0;
    /// Get the current renderer height
    virtual int GetHeight() const = 0;
    /// Get the WebClient associated with the render handler
    WebClient* GetWebClient() const;

    /// Set the dimensions of the render handler
    virtual void SetSize(int width, int height) = 0;

    /// Set the render handlers WebClient
    void SetWebClient(WebClient* webClient);

    /// Get the (internal) CEFRenderHandler
    virtual CefRenderHandler* GetCEFRenderHandler() = 0;

protected:

    WeakPtr<WebClient> webClient_;


};

}
