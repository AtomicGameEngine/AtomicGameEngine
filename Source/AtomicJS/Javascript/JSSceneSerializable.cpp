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

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/IO/File.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Scene.h>

// These serialization functions need to operate on various script classes
// including JS and C#, so use the base classes and avoid bringing in derived specifics
#include <Atomic/Script/ScriptComponent.h>
#include <Atomic/Script/ScriptComponentFile.h>

#include "JSScene.h"

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

    duk_push_this(ctx);
    Serializable* serial = js_to_class_instance<Serializable>(ctx, -1, 0);

    const Vector<AttributeInfo>* attributes = serial->GetAttributes();

    VariantType variantType = VAR_NONE;

    bool isAttr = false;

    if (attributes)
    {
        for (unsigned i = 0; i < attributes->Size(); i++)
        {
            const AttributeInfo* attr = &attributes->At(i);

            if (!attr->name_.Compare(name))
            {
                isAttr = true;
                variantType = attr->type_;
                break;
            }
        }
    }


    Variant v;
    js_to_variant(ctx, 1, v, variantType);

    ScriptComponent* jsc = NULL;

    // check dynamic
    if (!isAttr)
    {
        if (serial->GetBaseType() == ScriptComponent::GetTypeStatic())
        {

            jsc = (ScriptComponent*) serial;
            ScriptComponentFile* file = jsc->GetComponentFile();

            if (file)
            {
                const String& className = jsc->GetComponentClassName();

                const HashMap<String, VariantType>& fields = file->GetFields(className);
                const HashMap<String, Vector<EnumInfo>>& enums = file->GetEnums(className);

                if (VariantType *fvType = fields[name])
                {
                    variantType = *fvType;

                    if (enums.Contains(name))
                    {
                        int idx = (int) v.GetFloat();

                        if (idx > 0 && idx < enums[name]->Size())
                        {
                            VariantMap& values = jsc->GetFieldValues();
                            values[name] = enums[name]->At(idx).value_;
                            return 0;
                        }
                    }
                }
            }
        }
    }

    if (variantType == VAR_NONE)
        return 0;

    if (variantType == VAR_QUATERNION)
    {
        Vector3 v3 = v.GetVector3();
        Quaternion q;
        q.FromEulerAngles(v3.x_, v3.y_, v3.z_);
        v = q;
    }

    else if (variantType == VAR_COLOR)
    {
        Vector4 v4 = v.GetVector4();
        Color c(v4.x_, v4.y_, v4.z_, v4.w_ );
        v = c;
    }
    else if (variantType == VAR_INT)
    {
        v = (int) v.GetFloat();
    }
    else if (variantType == VAR_RESOURCEREF)
    {
        RefCounted* ref = v.GetPtr();

        if (ref && ref->IsObject())
        {
            Object* o = (Object*) ref;

            // TODO: calling code must ensure we are a resource, can this be done here?
            Resource* resource = (Resource*) o;

            v = ResourceRef(resource->GetType(), resource->GetName());

        }

    }    

    if (isAttr)
    {
        serial->SetAttribute(name, v);
        return 0;
    }

    // check dynamic
    if (jsc)
    {
        VariantMap& values = jsc->GetFieldValues();
        values[name] = v;
    }

    return 0;
}

static int Serializable_GetAttribute(duk_context* ctx)
{
    const char* name = duk_to_string(ctx, 0);

    duk_push_this(ctx);
    Serializable* serial = js_to_class_instance<Serializable>(ctx, -1, 0);
    const Vector<AttributeInfo>* attrs = serial->GetAttributes();

    if (attrs)
    {
        for (unsigned i = 0; i < attrs->Size(); i++)
        {
            const AttributeInfo* attr = &attrs->At(i);

            if (!attr->name_.Compare(name))
            {
                // FIXME: this is a double lookup
                js_push_variant(ctx,  serial->GetAttribute(name));
                return 1;
            }
        }
    }

    if (serial->GetBaseType() == ScriptComponent::GetTypeStatic())
    {
        ScriptComponent* jsc = (ScriptComponent*) serial;
        ScriptComponentFile* file = jsc->GetComponentFile();

        if (file)
        {
            const String& componentClassName = jsc->GetComponentClassName();

            const FieldMap& fields = file->GetFields(componentClassName);

            if (fields.Contains(name))
            {
                const VariantMap& values = jsc->GetFieldValues();

                if (Variant* vptr = values[name])
                {
                    js_push_variant(ctx,  *vptr);
                    return 1;
                }
                else
                {
                    Variant v;
                    file->GetDefaultFieldValue(name, v, componentClassName);
                    js_push_variant(ctx,  v);
                    return 1;
                }
            }
        }
    }

    duk_push_undefined(ctx);
    return 1;
}

static void GetDynamicAttributes(duk_context* ctx, unsigned& count, const VariantMap& defaultFieldValues,
                                 const FieldMap& fields,
                                 const EnumMap& enums)
{
    if (fields.Size())
    {
        HashMap<String, VariantType>::ConstIterator itr = fields.Begin();
        while (itr != fields.End())
        {
            duk_push_object(ctx);

            duk_push_number(ctx, (double) itr->second_);
            duk_put_prop_string(ctx, -2, "type");

            if (itr->second_ == VAR_RESOURCEREF && defaultFieldValues.Contains(itr->first_))
            {
                if (defaultFieldValues[itr->first_]->GetType() == VAR_RESOURCEREF)
                {
                    const ResourceRef& ref = defaultFieldValues[itr->first_]->GetResourceRef();
                    const String& typeName = JSVM::GetJSVM(ctx)->GetContext()->GetTypeName(ref.type_);

                    if (typeName.Length())
                    {
                        duk_push_string(ctx, typeName.CString());
                        duk_put_prop_string(ctx, -2, "resourceTypeName");

                    }
                }
            }

            duk_push_string(ctx, itr->first_.CString());
            duk_put_prop_string(ctx, -2, "name");

            duk_push_number(ctx, (double) AM_DEFAULT);
            duk_put_prop_string(ctx, -2, "mode");

            duk_push_string(ctx,"");
            duk_put_prop_string(ctx, -2, "defaultValue");

            duk_push_boolean(ctx, 1);
            duk_put_prop_string(ctx, -2, "dynamic");

            duk_push_array(ctx);

            if (enums.Contains(itr->first_))
            {
                unsigned enumCount = 0;
                const Vector<EnumInfo>* infos = enums[itr->first_];
                Vector<EnumInfo>::ConstIterator eitr = infos->Begin();

                while (eitr != infos->End())
                {
                    duk_push_string(ctx, eitr->name_.CString());
                    duk_put_prop_index(ctx, -2, enumCount++);
                    eitr++;
                }

            }

            duk_put_prop_string(ctx, -2, "enumNames");

            // store attr object
            duk_put_prop_index(ctx, -2, count++);

            itr++;
        }
    }
}



static int Serializable_GetAttributes(duk_context* ctx)
{
    duk_push_this(ctx);
    Serializable* serial = js_to_class_instance<Serializable>(ctx, -1, 0);
    unsigned type = serial->GetType().Value();

    duk_get_global_string(ctx, "__atomic_scene_serializable_attributes");
    duk_get_prop_index(ctx, -1, type);

    // return cached array of attrinfo, unless JSComponent which has dynamic fields
    if (serial->GetBaseType() != ScriptComponent::GetTypeStatic() && duk_is_object(ctx, -1))
        return 1;

    const Vector<AttributeInfo>* attrs = serial->GetAttributes();

    duk_push_array(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_index(ctx, -4, type);

    unsigned count = 0;
    if (attrs)
    {
        count = attrs->Size();
        for (unsigned i = 0; i < attrs->Size(); i++)
        {
            const AttributeInfo* attr = &attrs->At(i);

            if (attr->mode_ & AM_NOEDIT)
                continue;

            duk_push_object(ctx);

            duk_push_number(ctx, (double) attr->type_);
            duk_put_prop_string(ctx, -2, "type");

            if (attr->type_ == VAR_RESOURCEREF)
            {
                if (attr->defaultValue_.GetType() == VAR_RESOURCEREF)
                {
                    const ResourceRef& ref = attr->defaultValue_.GetResourceRef();
                    const String& typeName = serial->GetContext()->GetTypeName(ref.type_);

                    if (typeName.Length())
                    {
                        duk_push_string(ctx, typeName.CString());
                        duk_put_prop_string(ctx, -2, "resourceTypeName");

                    }
                }

            }

            if (attr->type_ == VAR_RESOURCEREFLIST)
            {
                if (attr->defaultValue_.GetType() == VAR_RESOURCEREFLIST)
                {
                    const ResourceRefList& ref = attr->defaultValue_.GetResourceRefList();
                    const String& typeName = serial->GetContext()->GetTypeName(ref.type_);

                    if (typeName.Length())
                    {
                        duk_push_string(ctx, typeName.CString());
                        duk_put_prop_string(ctx, -2, "resourceTypeName");

                    }
                }

            }


            duk_push_string(ctx, attr->name_.CString());
            duk_put_prop_string(ctx, -2, "name");

            duk_push_number(ctx, (double) attr->mode_);
            duk_put_prop_string(ctx, -2, "mode");

            duk_push_string(ctx,attr->defaultValue_.ToString().CString());
            duk_put_prop_string(ctx, -2, "defaultValue");

            duk_push_boolean(ctx, 0);
            duk_put_prop_string(ctx, -2, "dynamic");

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
    }

    // dynamic script fields
    if (serial->GetBaseType() == ScriptComponent::GetTypeStatic())
    {
        ScriptComponent* jsc = (ScriptComponent*) serial;
        ScriptComponentFile* file = jsc->GetComponentFile();

        if (file)
        {

            const String& className = jsc->GetComponentClassName();
            const VariantMap& defaultFieldValues = file->GetDefaultFieldValues(className);
            const FieldMap& fields =  file->GetFields(className);
            const EnumMap& enums = file->GetEnums(className);

            GetDynamicAttributes(ctx, count, defaultFieldValues, fields, enums);
        }
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
