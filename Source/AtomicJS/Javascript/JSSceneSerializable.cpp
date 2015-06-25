// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/IO/File.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Scene.h>

#include "JSScene.h"
#include "JSComponent.h"
#include "JSVM.h"

namespace Atomic
{

/*
    /// Attribute type.
    VariantType type_;
    /// Name.
    String name_;
    /// Byte offset from start of object.
    unsigned offset_;
    /// Enum names.
    const char** enumNames_;
    /// Helper object for accessor mode.
    SharedPtr<AttributeAccessor> accessor_;
    /// Default value for network replication.
    Variant defaultValue_;
    /// Attribute mode: whether to use for serialization, network replication, or both.
    unsigned mode_;
    /// Attribute data pointer if elsewhere than in the Serializable.
    void* ptr_;

*/

static int Serializable_SetAttribute(duk_context* ctx)
{
    const char* name = duk_to_string(ctx, 0);
    Variant v;
    js_to_variant(ctx, 1, v);

    duk_push_this(ctx);
    Serializable* serial = js_to_class_instance<Serializable>(ctx, -1, 0);

    const Vector<AttributeInfo>* attributes = serial->GetAttributes();

    for (unsigned i = 0; i < attributes->Size(); i++)
    {
        const AttributeInfo* attr = &attributes->At(i);

        if (attr->name_ == name)
        {
            if (attr->type_ == VAR_QUATERNION)
            {
                Vector3 v3 = v.GetVector3();
                Quaternion q;
                q.FromEulerAngles(v3.x_, v3.y_, v3.z_);
                v = q;
            }

            else if (attr->type_ == VAR_COLOR)
            {
                Vector4 v4 = v.GetVector4();
                Color c(v4.x_, v4.y_, v4.z_, v4.w_ );
                v = c;
            }
            else if (attr->type_ == VAR_INT)
            {
                v = (int) v.GetFloat();
            }

            break;
        }
    }

    serial->SetAttribute(name, v);

    return 0;
}

static int Serializable_GetAttribute(duk_context* ctx)
{
    const char* name = duk_to_string(ctx, 0);

    duk_push_this(ctx);
    Serializable* serial = js_to_class_instance<Serializable>(ctx, -1, 0);
    js_push_variant(ctx,  serial->GetAttribute(name));

    return 1;
}


static int Serializable_GetAttributes(duk_context* ctx)
{
    duk_push_this(ctx);
    Serializable* serial = js_to_class_instance<Serializable>(ctx, -1, 0);
    unsigned type = serial->GetType().Value();

    duk_get_global_string(ctx, "__atomic_scene_serializable_attributes");
    duk_get_prop_index(ctx, -1, type);

    // return cached array of attrinfo
    if (duk_is_object(ctx, -1))
        return 1;

    const Vector<AttributeInfo>* attrs = serial->GetAttributes();

    duk_push_array(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_index(ctx, -4, type);

    if (!attrs)
        return 1;

    for (unsigned i = 0; i < attrs->Size(); i++)
    {
        const AttributeInfo* attr = &attrs->At(i);

        if (attr->mode_ & AM_NOEDIT)
            continue;

        duk_push_object(ctx);

        duk_push_number(ctx, (double) attr->type_);
        duk_put_prop_string(ctx, -2, "type");

        duk_push_string(ctx, attr->name_.CString());
        duk_put_prop_string(ctx, -2, "name");

        duk_push_number(ctx, (double) attr->mode_);
        duk_put_prop_string(ctx, -2, "mode");

        duk_push_string(ctx,attr->defaultValue_.ToString().CString());
        duk_put_prop_string(ctx, -2, "defaultValue");

        duk_push_array(ctx);

        const char** enumPtr = attr->enumNames_;
        unsigned enumCount = 0;

        if (enumPtr)
        {
            while (*enumPtr)
            {
                duk_push_string(ctx, *enumPtr);
                duk_put_prop_index(ctx, -2, enumCount++);
                enumPtr++;
            }
        }

        duk_put_prop_string(ctx, -2, "enumNames");

        // store attr object
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;
}

void jsapi_init_scene_serializable(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    // cached attr
    duk_push_object(ctx);
    duk_put_global_string(ctx, "__atomic_scene_serializable_attributes");

    js_class_get_prototype(ctx, "Atomic", "Serializable");
    duk_push_c_function(ctx, Serializable_GetAttributes, 0);
    duk_put_prop_string(ctx, -2, "getAttributes");
    duk_push_c_function(ctx, Serializable_GetAttribute, 1);
    duk_put_prop_string(ctx, -2, "getAttribute");
    duk_push_c_function(ctx, Serializable_SetAttribute, 2);
    duk_put_prop_string(ctx, -2, "setAttribute");
    duk_pop(ctx);

}

}
