// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "JSIO.h"
#include "JSVM.h"
#include <Atomic/IO/File.h>

namespace Atomic
{

static int File_ReadText(duk_context* ctx)
{
    duk_push_this(ctx);
    File* file = js_to_class_instance<File>(ctx, -1, 0);

    String text;
    file->ReadText(text);

    duk_push_string(ctx, text.CString());

    return 1;
}

void jsapi_init_io(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "Atomic", "File");
    duk_push_c_function(ctx, File_ReadText, 0);
    duk_put_prop_string(ctx, -2, "readText");
    duk_pop(ctx);

}

}
