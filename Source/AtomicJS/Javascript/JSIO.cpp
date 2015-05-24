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

static int FileSystem_ScanDir(duk_context* ctx)
{
    duk_push_this(ctx);

    FileSystem* fs = js_to_class_instance<FileSystem>(ctx, -1, 0);

    if ( !duk_is_string(ctx, 0) || !duk_is_string(ctx, 1) ||
            !duk_is_number(ctx, 2) || !duk_is_boolean(ctx, 3))
    {
        duk_push_string(ctx, "FileSystem::ScanDir bad args");
        duk_throw(ctx);
    }

    const char* pathName = duk_to_string(ctx, 0);
    const char* filter = duk_to_string(ctx, 1);
    unsigned flags = duk_to_number(ctx, 2);
    bool recursive = duk_to_boolean(ctx, 3) ? true : false;

    Vector<String> result;

    fs->ScanDir(result, pathName, filter, flags, recursive);

    duk_push_array(ctx);

    for (unsigned i = 0; i < result.Size(); i++)
    {
        duk_push_string(ctx, result[i].CString());
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;
}

void jsapi_init_io(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "File");
    duk_push_c_function(ctx, File_ReadText, 0);
    duk_put_prop_string(ctx, -2, "readText");
    duk_pop(ctx);

    js_class_get_prototype(ctx, "FileSystem");
    duk_push_c_function(ctx, FileSystem_ScanDir, 4);
    duk_put_prop_string(ctx, -2, "scanDir");
    duk_pop(ctx);

}

}
