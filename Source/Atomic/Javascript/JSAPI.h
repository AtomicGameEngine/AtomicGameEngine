
#pragma once

#include <Duktape/duktape.h>

typedef void* JS_HEAP_PTR;

#include "../Javascript/JSVM.h"

#define JS_GLOBALSTASH_INDEX_COMPONENTS 0
#define JS_GLOBALSTASH_INDEX_NODE_REGISTRY 1

// indexers for instance objects
#define JS_INSTANCE_INDEX_FINALIZED 0

namespace Atomic
{

class JSVM;
class Object;

void js_constructor_basecall(duk_context* ctx, const char* baseclass);
void js_class_declare(JSVM* vm, const char* classname, duk_c_function constructor);
void js_setup_prototype(JSVM* vm, const char* classname, const char* basename, bool hasProperties = false);
void js_class_push_propertyobject(JSVM* vm, const char* classname);
void js_class_get_prototype(duk_context* ctx, const char *classname);

}
