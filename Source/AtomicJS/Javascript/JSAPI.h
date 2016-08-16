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

#include <Duktape/duktape.h>

typedef void* JS_HEAP_PTR;

#include "../Javascript/JSVM.h"

#define JS_GLOBALSTASH_INDEX_REFCOUNTED_REGISTRY 0
#define JS_GLOBALSTASH_VARIANTMAP_CACHE 1

// indexers for instance objects
#define JS_INSTANCE_INDEX_FINALIZED 0

namespace Atomic
{

class JSVM;
class Object;

void js_class_declare_internal(JSVM* vm, void* uniqueClassID, const char* package, const char* classname, duk_c_function constructor);

template<typename T>
void js_class_declare(JSVM* vm, const char* package, const char* classname, duk_c_function constructor)
{
    js_class_declare_internal(vm, (void*) T::GetClassIDStatic(), package, classname, constructor);
}

void js_constructor_basecall(duk_context* ctx, const char* package, const char* baseclass);
void js_setup_prototype(JSVM* vm, const char* package, const char* classname, const char* basePackage, const char* basename, bool hasProperties = false);
void js_class_push_propertyobject(JSVM* vm, const char* package, const char* classname);
void js_class_get_prototype(duk_context* ctx, const char* package, const char *classname);
void js_class_get_constructor(duk_context* ctx, const char* package, const char *classname);

/// Pushes variant value or undefined if can't be pushed
void js_push_variant(duk_context* ctx, const Variant &v);
void js_push_variantmap(duk_context* ctx, const VariantMap &vmap);

void js_to_variant(duk_context* ctx, int variantIdx, Variant &v, VariantType variantType = VAR_NONE);

void js_object_to_variantmap(duk_context* ctx, int objIdx, VariantMap &v);

/// Returns true if the item is a buffer, and if data and size are passed, they are given values to access the buffer data.
duk_bool_t js_check_is_buffer_and_get_data(duk_context* ctx, duk_idx_t idx, void** data, duk_size_t* size);

}
