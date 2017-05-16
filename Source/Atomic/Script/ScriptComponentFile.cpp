//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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


#include "ScriptComponentFile.h"

namespace Atomic
{

FieldMap ScriptComponentFile::emptyFieldMap_;
FieldTooltipMap ScriptComponentFile::emptyFieldTooltipMap_;
EnumMap ScriptComponentFile::emptyEnumMap_;
VariantMap ScriptComponentFile::emptyDefaultValueMap_;

ScriptComponentFile::ScriptComponentFile(Context* context) : Resource(context)
{

}

ScriptComponentFile::~ScriptComponentFile()
{

}

void ScriptComponentFile::RegisterObject(Context* context)
{
    
}

void ScriptComponentFile::AddEnum(const String& enumName, const EnumInfo& enumInfo, const String& classname)
{
    EnumMap& enums = classEnums_[classname];
    Vector<EnumInfo>& enumValues = enums[enumName];
    enumValues.Push(enumInfo);
}

void ScriptComponentFile::AddField(const String& fieldName, VariantType variantType, const String& resourceTypeName, bool isArray, bool isEnum, unsigned fixedArraySize, const String &classname, const String& tooltip)
{
    FieldMap& fields = classFields_[classname];
    FieldInfo finfo(fieldName, variantType, resourceTypeName, isArray, isEnum, fixedArraySize);
    fields[fieldName] = finfo;

    if (tooltip.Length())
    {
        FieldTooltipMap& tooltips = classFieldTooltips_[classname];
        tooltips[fieldName] = tooltip;
    }
}

void ScriptComponentFile::AddDefaultValue(const String& fieldName, const Variant& value, const String& classname)
{
    VariantMap& defaultValues = classDefaultFieldValues_[classname];
    defaultValues[fieldName] = value;
}

void ScriptComponentFile::Clear()
{
    classFields_.Clear();
    classFieldTooltips_.Clear();
    classDefaultFieldValues_.Clear();
    classEnums_.Clear();
}

const FieldMap& ScriptComponentFile::GetFields(const String& classname) const
{
    FieldMap* fieldMap = classFields_[classname];

    if (fieldMap)
        return *fieldMap;

    return emptyFieldMap_;
}

const FieldTooltipMap& ScriptComponentFile::GetFieldTooltips(const String& classname) const
{
    FieldTooltipMap* fieldTooltipMap = classFieldTooltips_[classname];

    if (fieldTooltipMap)
        return *fieldTooltipMap;

    return emptyFieldTooltipMap_;
}


const VariantMap& ScriptComponentFile::GetDefaultFieldValues(const String& classname) const
{
    VariantMap *vmap = classDefaultFieldValues_[classname];

    if (vmap)
        return *vmap;

    return emptyDefaultValueMap_;
}

const EnumMap& ScriptComponentFile::GetEnums(const String& classname) const
{
    EnumMap* enumMap = classEnums_[classname];

    if (enumMap)
        return *enumMap;

    return emptyEnumMap_;
}

void ScriptComponentFile::GetDefaultFieldValue(const String& name, Variant& v,const String& classname) const
{
    v = Variant::EMPTY;

    // first see if we have a default value for this field
    const VariantMap* defaultValues = classDefaultFieldValues_[classname];

    if (!defaultValues)
        return;

    const Variant* variant = (*defaultValues)[name];

    if (variant)
    {
        v = *variant;
        return;
    }

    // we don't have a default value, so we need to generate one based on the field type

    const FieldMap* fieldMap = classFields_[classname];

    if (!fieldMap)
        return;

    const FieldInfo* finfo = (*fieldMap)[name];

    if (!finfo || finfo->isArray_)
        return;

    VariantType variantType = finfo->variantType_;

    if (!variantType)
        return;

    switch (variantType)
    {
    case VAR_BOOL:
        v = false;
        break;
    case VAR_INT:
        v = 0;
    case VAR_DOUBLE:
        v = 0;
    case VAR_STRING:
        v = "";
        break;
    case VAR_FLOAT:
        v = 0.0f;
        break;
    case VAR_VECTOR2:
        v = Vector2::ZERO;
        break;
    case VAR_VECTOR3:
        v = Vector3::ZERO;
        break;
    case VAR_VECTOR4:
        v = Vector4::ZERO;
        break;
    case VAR_INTVECTOR2:
        v = IntVector2::ZERO;
        break;
    case VAR_COLOR:
        v = Color::WHITE;
        break;
    case VAR_QUATERNION:
        v = Quaternion::IDENTITY;
        break;
    case VAR_RESOURCEREF:
        v = Quaternion::IDENTITY;
        break;
    default:
        break;
    }

}


}
