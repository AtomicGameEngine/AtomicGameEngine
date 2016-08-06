//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Copyright (c) 2008-2015 the Urho3D project.
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

#include "../Core/Object.h"
#include "../Container/Str.h"
#include "../Container/ArrayPtr.h"

namespace asio
{
class io_service;
}

#ifndef EMSCRIPTEN
extern "C"
{
typedef void CURLM;
}
#endif

namespace Atomic
{

/// HTTP connection state
enum WebRequestState
{
    HTTP_INITIALIZING = 0,
    HTTP_ERROR,
    HTTP_OPEN,
    HTTP_CLOSED
};

struct WebRequestInternalState;

/// An HTTP connection with response data stream.
class WebRequest : public Object
{
    friend class Web;

    ATOMIC_OBJECT(WebRequest, Object)

public:
    /// Construct with parameters.
    WebRequest(Context* context, const String& verb, const String& url, double requestContentSize);
    /// Destruct. Release the connection.
    ~WebRequest();

    /// Return URL used in the request.
    const String& GetURL() const;

    /// Return error. Only non-empty in the error state.
    String GetError() const;
    /// Return connection state.
    WebRequestState GetState() const;
    /// Get the HTTP verb for this request.
    String GetVerb() const;

    /// Abort the WebRequest.
    void Abort();

    /// Return whether connection is in the open state.
    bool IsOpen() const { return GetState() == HTTP_OPEN; }

    /// Return whether "download_chunk" event will be fired or if only "complete" will be.
    bool HasDownloadChunkEvent();

    /// Set an HTTP request header (only works before Send has been called).
    void SetRequestHeader(const String& header, const String& value);
    /// Start sending the request.
    void Send();
    /// Get an HTTP response header.
    StringVector GetResponseHeaderKeys();
    /// Get an HTTP response header.
    String GetResponseHeader(const String& header);
    /// Get all HTTP response headers. Using GetResponseHeaderKeys() and GetResponseHeader() is more efficient than using this function.
    String GetAllResponseHeaders();

private:
#ifndef EMSCRIPTEN
    void setup(asio::io_service *service, CURLM* curlm);
    static void internalNotify(WebRequest *wr, int code);
#endif
    WebRequestInternalState* is_;
};

}
