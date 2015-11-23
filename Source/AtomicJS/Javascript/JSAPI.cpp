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

#include <Atomic/Core/Context.h>
#include <Atomic/Resource/ResourceCache.h>

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

void js_class_get_prototype(duk_context* ctx, const char* package, const char *classname)
{
    duk_get_global_string(ctx, package);
    duk_get_prop_string(ctx, -1, classname);
    duk_get_prop_string(ctx, -1, "prototype");
    duk_remove(ctx, -2); // remove class object
    duk_remove(ctx, -2); // remove Atomic object
}

void js_class_get_constructor(duk_context* ctx, const char* package, const char *classname)
{
    duk_get_global_string(ctx, package);
    duk_get_prop_string(ctx, -1, classname);
    duk_remove(ctx, -2); // remove package
}

void js_constructor_basecall(duk_context* ctx, const char* package, const char* baseclass)
{
    int top = duk_get_top(ctx);
    duk_get_global_string(ctx, package);
    duk_get_prop_string(ctx, -1, baseclass);
    assert(duk_is_function(ctx, -1));
    duk_push_this(ctx);
    duk_call_method(ctx, 0);
    duk_pop_n(ctx, 2);
    assert (top == duk_get_top(ctx));
}

void js_class_declare_internal(JSVM* vm, void* uniqueClassID, const char* package, const char* classname, duk_c_function constructor)
{
    duk_context* ctx = vm->GetJSContext();

    // uniqueClassID must be non-null
    assert(uniqueClassID);

    // stash a lookup from the uniqueID to the package and class name

    duk_push_heap_stash(ctx);
    duk_push_pointer(ctx, uniqueClassID);

    duk_push_object(ctx);
    duk_push_string(ctx, package);
    duk_put_prop_index(ctx, -2, 0);
    duk_push_string(ctx, classname);
    duk_put_prop_index(ctx, -2, 1);

    // store class object into uniqueClassID key
    duk_put_prop(ctx, -3);

    // pop heap stash
    duk_pop(ctx);

    // store the constructor
    duk_get_global_string(ctx, package);
    duk_push_c_function(ctx, constructor, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, classname);
    duk_pop(ctx);
}

void js_class_push_propertyobject(JSVM* vm, const char* package, const char* classname)
{
    duk_context* ctx = vm->GetJSContext();
    String pname;
    pname.AppendWithFormat("__%s__Properties", classname);

    duk_get_global_string(ctx, package);
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, pname.CString());
    duk_remove(ctx, -2); // remove Atomic object
}

void js_setup_prototype(JSVM* vm, const char* package, const char* classname, const char* basePackage, const char* basename, bool hasProperties)
{
    duk_context* ctx = vm->GetJSContext();

    String pname;
    pname.AppendWithFormat("__%s__Properties", classname);

    int top = duk_get_top(ctx);

    duk_get_global_string(ctx,package);
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

    duk_get_global_string(ctx, basePackage);
    duk_get_prop_string(ctx, -1, basename);
    assert(duk_is_function(ctx, -1));
    duk_get_prop_string(ctx, -1, "prototype");

    assert(duk_is_object(ctx, -1));

    duk_remove(ctx, -2); // remove basename

    int numargs = 1;
    if (hasProperties)
    {
        duk_get_global_string(ctx, package);
        duk_get_prop_string(ctx, -1, pname.CString());
        assert(duk_is_object(ctx, -1));
        duk_remove(ctx, -2);
        duk_remove(ctx, -3); // remove package
        numargs++;
    }
    else
        duk_remove(ctx, -2); // remove package

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

void js_object_to_variantmap(duk_context* ctx, int objIdx, VariantMap &v)
{
    v.Clear();

    duk_enum(ctx, objIdx, DUK_ENUM_OWN_PROPERTIES_ONLY);

    while (duk_next(ctx, -1 /*enum_index*/, 1 /*get_value*/)) {

        /* [ ... enum key ] */

        const char* key = duk_to_string(ctx, -2);

        if (duk_is_number(ctx, -1)) {

            v[key] = (float) duk_to_number(ctx, -1);

        } else if (duk_is_boolean(ctx, -1)) {

            v[key] = duk_to_boolean(ctx, -1) ? true : false;

        }
        else if (duk_is_string(ctx, -1)) {

            v[key] = duk_to_string(ctx, -1);

        } else if (duk_get_heapptr(ctx, -1)) {

            v[key] = js_to_class_instance<Object>(ctx, -1, 0);

        }

        duk_pop_2(ctx);  /* pop_key & value*/
    }

    duk_pop(ctx);  /* pop enum object */

}

duk_bool_t js_check_is_buffer_and_get_data(duk_context* ctx, duk_idx_t idx, void** data, duk_size_t* size)
{
    void* temp;
    if (duk_is_buffer(ctx, idx))
    {
        temp = duk_get_buffer_data(ctx, idx, size);
        if (data)
        {
            *data = temp;
        }
        return true;
    }
    if (!(duk_is_object(ctx, idx) &&
        duk_has_prop_string(ctx, idx, "length") &&
        duk_has_prop_string(ctx, idx, "byteLength") &&
        duk_has_prop_string(ctx, idx, "byteOffset") &&
        duk_has_prop_string(ctx, idx, "BYTES_PER_ELEMENT")))
    {
        if (data)
        {
            *data = nullptr;
        }
        if (size)
        {
            *size = 0;
        }
        return false;
    }
    temp = duk_require_buffer_data(ctx, idx, size);
    if (data)
    {
        *data = temp;
    }
    return true;
}

void js_to_variant(duk_context* ctx, int variantIdx, Variant &v, VariantType variantType)
{
    v.Clear();

    // convert to abs index
    if (variantIdx < 0)
        variantIdx = duk_get_top(ctx) + variantIdx;

    if (duk_is_boolean(ctx, variantIdx))
    {
        v = duk_to_boolean(ctx, variantIdx) ? true : false;
        return;
    }

    if (duk_is_string(ctx, variantIdx))
    {
        v = duk_to_string(ctx, variantIdx);
        return;
    }

    if (duk_is_number(ctx, variantIdx))
    {
        v = (float) duk_to_number(ctx, variantIdx);
        return;
    }

    if (duk_is_pointer(ctx, variantIdx))
    {
        v = (RefCounted*) duk_get_pointer(ctx, variantIdx);
        return;
    }

    if (duk_is_array(ctx, variantIdx))
    {
        if (duk_get_length(ctx, variantIdx) == 2)
        {
            Vector2 v2;
            duk_get_prop_index(ctx, variantIdx, 0);
            v2.x_ = duk_to_number(ctx, -1);
            duk_get_prop_index(ctx, variantIdx, 1);
            v2.y_ = duk_to_number(ctx, -1);
            duk_pop_n(ctx, 2);
            v = v2;
            return;
        }
        else if (duk_get_length(ctx, variantIdx) == 3)
        {
            Vector3 v3;
            duk_get_prop_index(ctx, variantIdx, 0);
            v3.x_ = duk_to_number(ctx, -1);
            duk_get_prop_index(ctx, variantIdx, 1);
            v3.y_ = duk_to_number(ctx, -1);
            duk_get_prop_index(ctx, variantIdx, 2);
            v3.z_ = duk_to_number(ctx, -1);
            duk_pop_n(ctx, 3);
            v = v3;
            return;
        }
        else if (duk_get_length(ctx, variantIdx) == 4)
        {
            Vector4 v4;
            duk_get_prop_index(ctx, variantIdx, 0);
            v4.x_ = duk_to_number(ctx, -1);
            duk_get_prop_index(ctx, variantIdx, 1);
            v4.y_ = duk_to_number(ctx, -1);
            duk_get_prop_index(ctx, variantIdx, 2);
            v4.z_ = duk_to_number(ctx, -1);
            duk_get_prop_index(ctx, variantIdx, 3);
            v4.w_ = duk_to_number(ctx, -1);
            duk_pop_n(ctx, 4);
            v = v4;
            return;
        }


        return;
    }

    {
        void* bufferData;
        duk_size_t bufferSize;
        if (js_check_is_buffer_and_get_data(ctx, variantIdx, &bufferData, &bufferSize))
        {
            // copy the buffer into the variant
            v.SetBuffer(bufferData, (unsigned)bufferSize);
            return;
        }
    }

    // object check after array and buffer object check
    if (duk_is_object(ctx, variantIdx))
    {
        if (variantType == VAR_RESOURCEREFLIST)
        {
            ResourceRefList refList;

            duk_get_prop_string(ctx, variantIdx, "typeName");
            refList.type_ = duk_to_string(ctx, -1);

            duk_get_prop_string(ctx, variantIdx, "resources");
            int length = duk_get_length(ctx, -1);

            for (int i = 0; i < length; i++) {

                duk_get_prop_index(ctx, -1, i);

                Resource* resource = NULL;

                if (duk_is_object(ctx, -1))
                {
                     resource = js_to_class_instance<Resource>(ctx, -1, 0);

                }

                if (resource) {
                    refList.names_.Push(resource->GetName());
                }
                else
                    refList.names_.Push(String::EMPTY);

                duk_pop(ctx);
            }

            duk_pop_n(ctx, 2);

            v = refList;
        }
        else
        {
            RefCounted* o = js_to_class_instance<RefCounted>(ctx, variantIdx, 0);
            if (o)
                v = o;
        }

        return;
    }


}

// variant map Proxy getter, so we can convert access to string based
// member lookup, to string hash on the fly

static int variantmap_property_get(duk_context* ctx)
{
    // targ, key, recv

    if (duk_is_string(ctx, 1))
    {
        StringHash key = duk_to_string(ctx, 1);
        duk_get_prop_index(ctx, 0, (unsigned) key.Value());
        return 1;
    }

    duk_push_undefined(ctx);
    return 1;

}

// removes all keys from the variant map proxy target, REGARDLESS of key given for delete
// see (lengthy) note in JSEventDispatcher::EndSendEvent
static int variantmap_property_deleteproperty(duk_context* ctx)
{
    // deleteProperty: function (targ, key)

    duk_enum(ctx, 0, DUK_ENUM_OWN_PROPERTIES_ONLY);

    while (duk_next(ctx, -1, 0)) {
        duk_del_prop(ctx, 0);
    }

    duk_push_boolean(ctx, 1);
    return 1;

}


void js_push_variantmap(duk_context* ctx, const VariantMap &vmap)
{

    // setup proxy so we can map string
    duk_get_global_string(ctx, "Proxy");

    duk_push_object(ctx);

    VariantMap::ConstIterator itr = vmap.Begin();

    while (itr != vmap.End()) {

        js_push_variant(ctx, itr->second_);


        if (duk_is_undefined(ctx, -1)) {

            duk_pop(ctx);
        }
        else
        {
            duk_put_prop_index(ctx, -2, (unsigned) itr->first_.Value());
        }

        itr++;

    }

    // setup property handler
    duk_push_object(ctx);
    duk_push_c_function(ctx, variantmap_property_get, 3);
    duk_put_prop_string(ctx, -2, "get");
    duk_push_c_function(ctx, variantmap_property_deleteproperty, 2);
    duk_put_prop_string(ctx, -2, "deleteProperty");

    duk_new(ctx, 2);


}

void js_push_variant(duk_context *ctx, const Variant& v)
{
    switch (v.GetType())
    {
    case VAR_NONE:
        duk_push_undefined(ctx);
        break;

    case VAR_VOIDPTR:
        duk_push_null(ctx);
        break;

    case VAR_PTR:
    {
        RefCounted* ref = v.GetPtr();

        // if we're null or don't have any refs, return null
        if (!ref || !ref->Refs())
        {
            duk_push_null(ctx);
            break;
        }

        // check that class is supported
        duk_push_heap_stash(ctx);
        duk_push_pointer(ctx, (void*)ref->GetClassID());
        duk_get_prop(ctx, -2);

        if (!duk_is_object(ctx, -1))
        {
            duk_pop_2(ctx);
            duk_push_undefined(ctx);
        }
        else
        {
            duk_pop_2(ctx);
            js_push_class_object_instance(ctx, ref);
        }

    }   break;

    case VAR_RESOURCEREF:
    {
        const ResourceRef& resourceRef(v.GetResourceRef());
        ResourceCache* cache = JSVM::GetJSVM(ctx)->GetContext()->GetSubsystem<ResourceCache>();
        Resource* resource = cache->GetResource(resourceRef.type_, resourceRef.name_);
        js_push_class_object_instance(ctx, resource);
    }   break;

    case VAR_RESOURCEREFLIST:
    {

        const ResourceRefList& resourceRefList(v.GetResourceRefList());
        const Context* context = JSVM::GetJSVM(ctx)->GetContext();

        duk_push_object(ctx);
        duk_push_string(ctx, context->GetTypeName(resourceRefList.type_).CString());
        duk_put_prop_string(ctx, -2, "typeName");

        duk_push_array(ctx);

        ResourceCache* cache = context->GetSubsystem<ResourceCache>();

        for (unsigned i = 0; i < resourceRefList.names_.Size(); i++) {

            Resource* resource = cache->GetResource(resourceRefList.type_, resourceRefList.names_[i]);
            js_push_class_object_instance(ctx, resource);
            duk_put_prop_index(ctx, -2, i);
        }

        duk_put_prop_string(ctx, -2, "resources");

    } break;

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
    {
        const String& string(v.GetString());
        duk_push_lstring(ctx, string.CString(), string.Length());
    }   break;

    case VAR_BUFFER:
    {
        const PODVector<unsigned char>& buffer(v.GetBuffer()); // The braces are to scope this reference.
        duk_push_fixed_buffer(ctx, buffer.Size());
        duk_push_buffer_object(ctx, -1, 0, buffer.Size(), DUK_BUFOBJ_UINT8ARRAY);
        duk_replace(ctx, -2);
        unsigned char* data = (unsigned char*)duk_require_buffer_data(ctx, -1, (duk_size_t*)nullptr);
        memcpy(data, buffer.Buffer(), buffer.Size());
    }   break;

    case VAR_VECTOR2:
    {
        const Vector2& vector2(v.GetVector2());
        duk_push_array(ctx);
        duk_push_number(ctx, vector2.x_);
        duk_put_prop_index(ctx, -2, 0);
        duk_push_number(ctx, vector2.y_);
        duk_put_prop_index(ctx, -2, 1);
    }   break;

    case VAR_INTVECTOR2:
    {
        const IntVector2& intVector2(v.GetIntVector2());
        duk_push_array(ctx);
        duk_push_number(ctx, intVector2.x_);
        duk_put_prop_index(ctx, -2, 0);
        duk_push_number(ctx, intVector2.y_);
        duk_put_prop_index(ctx, -2, 1);
    }   break;

    case VAR_VECTOR3:
    {
        const Vector3& vector3(v.GetVector3());
        duk_push_array(ctx);
        duk_push_number(ctx, vector3.x_);
        duk_put_prop_index(ctx, -2, 0);
        duk_push_number(ctx, vector3.y_);
        duk_put_prop_index(ctx, -2, 1);
        duk_push_number(ctx, vector3.z_);
        duk_put_prop_index(ctx, -2, 2);
    }   break;

    case VAR_QUATERNION:
    {
        const Vector3& vector3(v.GetQuaternion().EulerAngles());
        duk_push_array(ctx);
        duk_push_number(ctx, vector3.x_);
        duk_put_prop_index(ctx, -2, 0);
        duk_push_number(ctx, vector3.y_);
        duk_put_prop_index(ctx, -2, 1);
        duk_push_number(ctx, vector3.z_);
        duk_put_prop_index(ctx, -2, 2);
    }   break;

    case VAR_COLOR:
    {
        const Color& color(v.GetColor());
        duk_push_array(ctx);
        duk_push_number(ctx, color.r_);
        duk_put_prop_index(ctx, -2, 0);
        duk_push_number(ctx, color.g_);
        duk_put_prop_index(ctx, -2, 1);
        duk_push_number(ctx, color.b_);
        duk_put_prop_index(ctx, -2, 2);
        duk_push_number(ctx, color.a_);
        duk_put_prop_index(ctx, -2, 3);
    }   break;

    case VAR_VECTOR4:
    {
        const Vector4& vector4(v.GetVector4());
        duk_push_array(ctx);
        duk_push_number(ctx, vector4.x_);
        duk_put_prop_index(ctx, -2, 0);
        duk_push_number(ctx, vector4.y_);
        duk_put_prop_index(ctx, -2, 1);
        duk_push_number(ctx, vector4.z_);
        duk_put_prop_index(ctx, -2, 2);
        duk_push_number(ctx, vector4.w_);
        duk_put_prop_index(ctx, -2, 3);
    }   break;

    default:
        duk_push_undefined(ctx);
        break;
    }


}


}
