// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Duktape/duktape.h>

typedef void* JS_HEAP_PTR;

#include "../Javascript/JSVM.h"

#define JS_GLOBALSTASH_INDEX_COMPONENTS 0
#define JS_GLOBALSTASH_INDEX_NODE_REGISTRY 1
#define JS_GLOBALSTASH_VARIANTMAP_CACHE 2

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

void js_to_variant(duk_context* ctx, int variantIdx, Variant &v);

void js_object_to_variantmap(duk_context* ctx, int objIdx, VariantMap &v);

}
