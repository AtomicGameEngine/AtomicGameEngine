
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
