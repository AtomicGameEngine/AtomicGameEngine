
#pragma once

#include "../Resource/Resource.h"
#include "../Core/Variant.h"

namespace Atomic
{

struct EnumInfo
{
    EnumInfo(const String& name = String::EMPTY, const Variant& v = Variant::EMPTY)
    {
        name_ = name;
        value_ = v;
    }

    String name_;
    Variant value_;
};

typedef HashMap<String, VariantType> FieldMap;
typedef HashMap<String, Vector<EnumInfo>> EnumMap;

typedef HashMap<StringHash, FieldMap> ClassFieldMap;
typedef HashMap<StringHash, EnumMap> ClassEnumMap;
typedef HashMap<StringHash, VariantMap> ClassDefaultValueMap;

/// NET Assembly resource.
class ATOMIC_API ScriptComponentFile : public Resource
{
    ATOMIC_OBJECT(ScriptComponentFile, Resource);

public:

    /// Construct.
    ScriptComponentFile(Context* context);
    /// Destruct.
    virtual ~ScriptComponentFile();

    static void RegisterObject(Context* context);

    const EnumMap& GetEnums(const String& classname = String::EMPTY) const;
    const FieldMap& GetFields(const String& classname = String::EMPTY) const;
    const VariantMap& GetDefaultFieldValues(const String& classname = String::EMPTY) const;

    void GetDefaultFieldValue(const String& name, Variant& v,const String& classname = String::EMPTY) const;

protected:

    void Clear();

    void AddEnum(const String& enumName, const EnumInfo& enumInfo, const String& classname = String::EMPTY);
    void AddField(const String& fieldName, VariantType variantType, const String& classname = String::EMPTY);
    void AddDefaultValue(const String& fieldName, const Variant& value, const String& classname = String::EMPTY);

private:

    ClassFieldMap classFields_;
    ClassDefaultValueMap classDefaultFieldValues_;
    ClassEnumMap classEnums_;

    static FieldMap emptyFieldMap_;
    static EnumMap emptyEnumMap_;
    static VariantMap emptyDefaultValueMap_;

};

}
