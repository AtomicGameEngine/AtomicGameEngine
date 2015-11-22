//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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
#include "../Container/ArrayPtr.h"
#include "../IO/VectorBuffer.h"

namespace Atomic
{

class WebRequest;
class WebSocket;

struct WebPrivate;

/// %Web subsystem. Manages HTTP requests and WebSocket communications.
class ATOMIC_API Web : public Object
{
    OBJECT(Web);

public:
    /// Construct.
    Web(Context* context);
    /// Destruct.
    ~Web();

    /// Perform an HTTP request to the specified URL. Empty verb defaults to a GET request. Return a request object which can be used to read the response data.
    SharedPtr<WebRequest> MakeWebRequest
        (const String& url, const String& verb = String::EMPTY, const Vector<String>& headers = Vector<String>(),
            const String& postData = String::EMPTY);
    /// Perform an WebSocket request to the specified URL. Return a WebSocket object which can be used to comunicate with the server.
    SharedPtr<WebSocket> MakeWebSocket(const String& url);

private:
    void internalUpdate(StringHash eventType, VariantMap& eventData);
    WebPrivate *d;
};

}
