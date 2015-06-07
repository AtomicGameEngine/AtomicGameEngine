#ifdef ATOMIC_NETWORK

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

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


