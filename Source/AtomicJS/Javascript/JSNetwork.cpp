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

#ifdef ATOMIC_NETWORK


#include "JSNetwork.h"
#include "JSVM.h"

#include <Atomic/Network/HttpRequest.h>

namespace Atomic
{

static int HttpRequest_Read(duk_context* ctx)
{
    duk_push_this(ctx);

    HttpRequest* request = js_to_class_instance<HttpRequest>(ctx, -1, 0);

    if (!request->GetAvailableSize())
    {
        duk_push_string(ctx, "");
        return 1;
    }

    PODVector<unsigned char> buffer(request->GetAvailableSize());

    request->Read(&buffer[0], buffer.Size());

    duk_push_lstring(ctx, (const char*) &buffer[0], buffer.Size());

    return 1;
}

void jsapi_init_network(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Atomic", "HttpRequest");
    duk_push_c_function(ctx, HttpRequest_Read, 0);
    duk_put_prop_string(ctx, -2, "read");
    duk_pop(ctx);

}

}


#endif
