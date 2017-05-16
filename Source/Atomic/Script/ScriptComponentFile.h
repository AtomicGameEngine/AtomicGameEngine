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


#pragma once

#include "../Resource/Resource.h"
#include "../Core/Variant.h"

namespace Atomic
{


struct FieldInfo
{
    FieldInfo()
    {
        name_ = "UNINITIALIZED_FIELDINFO";
        variantType_ = VAR_NONE;
        isArray_ = false;
        isEnum_ = false;
        fixedArraySize_ = 0;
    }

    FieldInfo(const String& name, VariantType variantType, const String& resourceTypeName = String::EMPTY, bool isArray = false, bool isEnum = false, unsigned fixedArraySize = 0)
    {
        name_ = name;
        variantType_ = variantType;
        resourceTypeName_ = resourceTypeName;
        isArray_ = isArray;
        isEnum_ = isEnum;
        fixedArraySize_ = fixedArraySize;

        // register field name as significant, for serialization
        StringHash::RegisterSignificantString(name);
    }

    String name_;
    VariantType variantType_;
    // for resource ref variants
    String resourceTypeName_;
    bool isArray_;
    bool isEnum_;
    unsigned fixedArraySize_;
};

struct EnumInfo
{
    EnumInfo(const String& name = String::EMPTY, int value = 0)
    {
        name_ = name;
        value_ = value;
    }

    String name_;
    int value_;
};

// TODO: these should be broken out into some class info structs, getting unwieldy
typedef HashMap<String, FieldInfo> FieldMap;
typedef HashMap<String, Vector<EnumInfo>> EnumMap;
typedef HashMap<String, String> FieldTooltipMap;

typedef HashMap<StringHash, FieldMap> ClassFieldMap;
typedef HashMap<StringHash, FieldTooltipMap> ClassFieldTooltipMap;
typedef HashMap<StringHash, EnumMap> ClassEnumMap;
typedef HashMap<StringHash, VariantMap> ClassDefaultValueMap;


/// NET Assembly resource.
class ATOMIC_API ScriptComponentFile : public Resource
{
    ATOMIC_OBJECT(ScriptComponentFile, Resource)

public:

    /// Construct.
    ScriptComponentFile(Context* context);
    /// Destruct.
    virtual ~ScriptComponentFile();

    static void RegisterObject(Context* context);

    /// Only valid in editor, as we don't inspect classnames at runtime
    virtual const Vector<String>& GetClassNames() { return classNames_; }
    const EnumMap& GetEnums(const String& classname = String::EMPTY) const;
    const FieldMap& GetFields(const String& classname = String::EMPTY) const;
    const FieldTooltipMap& GetFieldTooltips(const String& classname = String::EMPTY) const;
    const VariantMap& GetDefaultFieldValues(const String& classname = String::EMPTY) const;

    void GetDefaultFieldValue(const String& name, Variant& v,const String& classname = String::EMPTY) const;

protected:

    void Clear();

    void AddEnum(const String& enumName, const EnumInfo& enumInfo, const String& classname = String::EMPTY);
    void AddField(const String& fieldName, VariantType variantType, const String& resourceTypeName = String::EMPTY, bool isArray = false, bool isEnum = false, unsigned fixedArraySize = 0, const String& classname = String::EMPTY, const String& tooltip = String::EMPTY);
    void AddDefaultValue(const String& fieldName, const Variant& value, const String& classname = String::EMPTY);

    // only valid in editor
    Vector<String> classNames_;

private:

    ClassFieldMap classFields_;
    ClassFieldTooltipMap classFieldTooltips_;
    ClassDefaultValueMap classDefaultFieldValues_;
    ClassEnumMap classEnums_;

    static FieldMap emptyFieldMap_;
    static FieldTooltipMap emptyFieldTooltipMap_;
    static EnumMap emptyEnumMap_;
    static VariantMap emptyDefaultValueMap_;

};

}
