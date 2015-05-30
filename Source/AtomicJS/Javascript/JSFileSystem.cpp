// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Core/ProcessUtils.h>

#include "JSCore.h"
#include "JSEventHelper.h"
#include "JSVM.h"

namespace Atomic
{

static int Atomic_SplitPath(duk_context* ctx)
{
    String path = duk_require_string(ctx, 0);

    String pathName;
    String fileName;
    String ext;

    duk_push_object(ctx);

    SplitPath(path, pathName, fileName, ext);

    duk_push_string(ctx, pathName.CString());
    duk_put_prop_string(ctx, -2, "pathName");

    duk_push_string(ctx, fileName.CString());
    duk_put_prop_string(ctx, -2, "fileName");

    duk_push_string(ctx, ext.CString());
    duk_put_prop_string(ctx, -2, "ext");

    return 1;

}

static int Atomic_AddTrailingSlash(duk_context* ctx)
{
    String path = duk_require_string(ctx, 0);

    duk_push_string(ctx, AddTrailingSlash(path).CString());

    return 1;
}

static int Atomic_GetExtenstion(duk_context* ctx)
{
    String path = duk_require_string(ctx, 0);

    duk_push_string(ctx, GetExtension(path).CString());

    return 1;
}

static int Atomic_GetParentPath(duk_context* ctx)
{
    String path = duk_require_string(ctx, 0);

    duk_push_string(ctx, GetParentPath(path).CString());

    return 1;
}


void jsapi_init_filesystem(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    duk_get_global_string(ctx, "Atomic");

    duk_push_c_function(ctx, Atomic_SplitPath, 1);
    duk_put_prop_string(ctx, -2, "splitPath");

    duk_push_c_function(ctx, Atomic_AddTrailingSlash, 1);
    duk_put_prop_string(ctx, -2, "addTrailingSlash");

    duk_push_c_function(ctx, Atomic_GetParentPath, 1);
    duk_put_prop_string(ctx, -2, "getParentPath");

    duk_push_c_function(ctx, Atomic_GetExtenstion, 1);
    duk_put_prop_string(ctx, -2, "getExtension");

    duk_pop(ctx); // pop Atomic object
}

}
