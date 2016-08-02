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

#include <Atomic/Core/Object.h>

namespace Atomic
{

class WebClient;
class WebMessageHandlerPrivate;

class WebMessageHandler : public Object
{

    ATOMIC_OBJECT(WebMessageHandler, Object);

public:

    /// Construct.
    WebMessageHandler(Context* context);
    /// Destruct.
    virtual ~WebMessageHandler();

    void HandleDeferredResponse(double queryID, bool success, const String& response);

    /// Success callback
    void Success(const String& response = String::EMPTY);
    /// Failure callback
    void Failure(int errorCode, const String& errorMessage);

    /// Get the WebClient associated with this WebMessageHandler
    WebClient* GetWebClient() { return client_; }

    /// Set the WebClient associated with this WebMessageHandler
    void SetWebClient(WebClient* client) { client_ = client; }       

    /// Get the CefMessageRouterBrowserSide::Handler* as a opaque void*
    void* GetCefHandler() { return (void*) d_; }

private:

    WeakPtr<WebClient> client_;
    WebMessageHandlerPrivate* d_;

};

}
