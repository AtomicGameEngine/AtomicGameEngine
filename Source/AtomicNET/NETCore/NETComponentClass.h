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

#pragma once

#include <Atomic/Container/HashMap.h>
#include <Atomic/Core/Variant.h>
#include <Atomic/Resource/JSONValue.h>

#include <Atomic/Script/Script.h>

namespace Atomic
{

/// NET Assembly resource.
class ATOMIC_API NETComponentClass : public Object
{
    OBJECT(NETComponentClass);

public:

    /// Construct.
    NETComponentClass(Context* context);
    /// Destruct.
    virtual ~NETComponentClass();

    const String& GetName() { return name_; }

    bool ParseJSON(const JSONValue& json);

    const HashMap<String, VariantType>& GetFields() const { return fields_; }
    const VariantMap& GetDefaultFieldValues() const { return defaultFieldValues_; }
    const HashMap<String, Vector<EnumInfo>>& GetEnums() const { return enums_; }
    void GetDefaultFieldValue(const String& name, Variant& v);

private:

    static void InitTypeMap();

    String name_;

    static HashMap<String, VariantType> typeMap_;

    HashMap<String, VariantType> fields_;
    VariantMap defaultFieldValues_;
    HashMap<String, Vector<EnumInfo>> enums_;


};

}
