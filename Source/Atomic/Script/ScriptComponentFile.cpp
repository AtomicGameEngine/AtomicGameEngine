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
    //context->RegisterFactory<ScriptComponentFile>();
}

void ScriptComponentFile::AddEnum(const String& enumName, const EnumInfo& enumInfo, const String& classname)
{
    EnumMap& enums = classEnums_[classname];
    Vector<EnumInfo>& enumValues = enums[enumName];
    enumValues.Push(enumInfo);
}

void ScriptComponentFile::AddField(const String& fieldName, VariantType variantType, const String &classname)
{
    FieldMap& fields = classFields_[classname];
    fields[fieldName] = variantType;
}

void ScriptComponentFile::AddDefaultValue(const String& fieldName, const Variant& value, const String& classname)
{
    VariantMap& defaultValues = classDefaultFieldValues_[classname];
    defaultValues[fieldName] = value;
}

void ScriptComponentFile::Clear()
{
    classFields_.Clear();
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

    const VariantType* variantType = (*fieldMap)[name];
    if (!variantType)
        return;

    switch (*variantType)
    {
    case VAR_BOOL:
        v = false;
        break;
    case VAR_STRING:
        v = "";
        break;
    case VAR_FLOAT:
        v = 0.0f;
        break;
    case VAR_VECTOR3:
        v = Vector3::ZERO;
        break;
    default:
        break;
    }

}


}
