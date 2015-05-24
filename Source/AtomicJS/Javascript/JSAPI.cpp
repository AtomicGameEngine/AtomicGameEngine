// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "JSAPI.h"
#include "JSVM.h"

/*

  // script can new an instance
  // script can get an engine ptr back (which may be to a previously script new'd instance)
  // script should always get the same JS Object back which means a lookup table (strong ref)
  // we can look directly at the tval (exposed from duktape internals) to see if the reference count is 1

  // script owned
  // new'd script side, script holds a RefCounted reference

  // native owned

  // native script classes not derived from refcounted
  // if not inherited from RefCounted can script access?
  // if so, script side will always need to delete?

*/

namespace Atomic
{

void js_class_get_prototype(duk_context* ctx, const char* classname)
{
    duk_get_global_string(ctx, "Atomic");
    duk_get_prop_string(ctx, -1, classname);
    duk_get_prop_string(ctx, -1, "prototype");
    duk_remove(ctx, -2); // remove class object
    duk_remove(ctx, -2); // remove Atomic object
}

void js_constructor_basecall(duk_context* ctx, const char* baseclass)
{
    int top = duk_get_top(ctx);
    duk_get_global_string(ctx, "Atomic");
    duk_get_prop_string(ctx, -1, baseclass);
    assert(duk_is_function(ctx, -1));
    duk_push_this(ctx);
    duk_call_method(ctx, 0);
    duk_pop_n(ctx, 2);
    assert (top == duk_get_top(ctx));
}

void js_class_declare(JSVM* vm, const char* classname, duk_c_function constructor)
{
    duk_context* ctx = vm->GetJSContext();
    duk_get_global_string(ctx, "Atomic");
    duk_push_c_function(ctx, constructor, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, classname);
    duk_pop(ctx);
}

void js_class_push_propertyobject(JSVM* vm, const char* classname)
{
    duk_context* ctx = vm->GetJSContext();
    String pname;
    pname.AppendWithFormat("__%s__Properties", classname);

    duk_get_global_string(ctx, "Atomic");
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, pname.CString());
    duk_remove(ctx, -2); // remove Atomic object
}



void js_setup_prototype(JSVM* vm, const char* classname, const char* basename, bool hasProperties)
{
    duk_context* ctx = vm->GetJSContext();

    String pname;
    pname.AppendWithFormat("__%s__Properties", classname);

    int top = duk_get_top(ctx);

    duk_get_global_string(ctx, "Atomic");
    duk_get_prop_string(ctx, -1, classname);
    assert(duk_is_c_function(ctx, -1));

    if (!strlen(basename))
    {
        // prototype
        duk_push_object(ctx);
        duk_dup(ctx, -2); // AObject constructor function
        duk_put_prop_string(ctx, -2, "constructor");
        duk_put_prop_string(ctx, -2, "prototype");

        duk_pop_n(ctx, 2);

        assert (top == duk_get_top(ctx));

        return;

    }

    // prototype
    duk_get_global_string(ctx, "Object");
    duk_get_prop_string(ctx, -1, "create");

    assert(duk_is_function(ctx, -1));

    duk_remove(ctx, -2); // remove Object

    duk_get_global_string(ctx, "Atomic");
    duk_get_prop_string(ctx, -1, basename);
    assert(duk_is_function(ctx, -1));
    duk_get_prop_string(ctx, -1, "prototype");

    assert(duk_is_object(ctx, -1));

    duk_remove(ctx, -2); // remove basename

    int numargs = 1;
    if (hasProperties)
    {
        duk_get_prop_string(ctx, -2, pname.CString());
        assert(duk_is_object(ctx, -1));
        duk_remove(ctx, -3); // remove Atomic
        numargs++;
    }
    else
        duk_remove(ctx, -2); // remove Atomic

    duk_call(ctx, numargs);

    assert(duk_is_object(ctx, -1));

    duk_dup(ctx, -2);
    duk_put_prop_string(ctx, -2, "constructor");

    //duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -2, "prototype");

    // pop the classname object
    duk_pop(ctx);

    // pop the Atomic Object
    duk_pop(ctx);

    assert (top == duk_get_top(ctx));
}

void js_push_variant(duk_context *ctx, const Variant& v)
{
    VariantType type = v.GetType();
    RefCounted* ref;
    Object* object;
    Vector2& vector2 = (Vector2&) Vector2::ZERO;
    Vector3& vector3 = (Vector3&) Vector3::ZERO;

    switch (type)
    {
    case VAR_PTR:

        ref = v.GetPtr();

        if (!ref || !ref->IsObject())
        {
            duk_push_null(ctx);
            break;
        }

        object = (Object*) ref;

        // check that class is supported
        duk_get_global_string(ctx, "Atomic");

        // will not handle renamed classes!
        duk_get_prop_string(ctx, -1, object->GetTypeName().CString());

        if (!duk_is_function(ctx, -1))
            object = NULL;

        duk_pop_n(ctx, 2);

        if (object)
            js_push_class_object_instance(ctx, object);
        else
            duk_push_undefined(ctx);
        break;

    case VAR_BOOL:
        duk_push_boolean(ctx, v.GetBool() ? 1 : 0);
        break;
    case VAR_INT:
        duk_push_number(ctx, v.GetInt());
        break;
    case VAR_FLOAT:
        duk_push_number(ctx, v.GetFloat());
        break;
    case VAR_STRING:
        duk_push_string(ctx, v.GetString().CString());
        break;
    case VAR_VECTOR2:
        vector2 = v.GetVector2();
        duk_push_array(ctx);
        duk_push_number(ctx, vector2.x_);
        duk_put_prop_index(ctx, -1, 0);
        duk_push_number(ctx, vector2.y_);
        duk_put_prop_index(ctx, -1, 1);
        break;
    case VAR_VECTOR3:
        vector3 = v.GetVector3();
        duk_push_array(ctx);
        duk_push_number(ctx, vector3.x_);
        duk_put_prop_index(ctx, -1, 0);
        duk_push_number(ctx, vector3.y_);
        duk_put_prop_index(ctx, -1, 1);
        duk_push_number(ctx, vector3.z_);
        duk_put_prop_index(ctx, -1, 1);
        break;

    default:
        duk_push_undefined(ctx);
        break;
    }


}


}

