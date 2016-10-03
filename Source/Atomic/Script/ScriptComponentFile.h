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

    /// Only valid in editor, as we don't inspect classnames at runtime
    virtual const Vector<String>& GetClassNames() { return classNames_; }
    const EnumMap& GetEnums(const String& classname = String::EMPTY) const;
    const FieldMap& GetFields(const String& classname = String::EMPTY) const;
    const VariantMap& GetDefaultFieldValues(const String& classname = String::EMPTY) const;

    void GetDefaultFieldValue(const String& name, Variant& v,const String& classname = String::EMPTY) const;

protected:

    void Clear();

    void AddEnum(const String& enumName, const EnumInfo& enumInfo, const String& classname = String::EMPTY);
    void AddField(const String& fieldName, VariantType variantType, const String& classname = String::EMPTY);
    void AddDefaultValue(const String& fieldName, const Variant& value, const String& classname = String::EMPTY);

    // only valid in editor
    Vector<String> classNames_;

private:

    ClassFieldMap classFields_;
    ClassDefaultValueMap classDefaultFieldValues_;
    ClassEnumMap classEnums_;

    static FieldMap emptyFieldMap_;
    static EnumMap emptyEnumMap_;
    static VariantMap emptyDefaultValueMap_;

};

}
