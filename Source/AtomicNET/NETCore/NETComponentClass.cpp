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

#include <Atomic/IO/Log.h>
#include "NETAssemblyFile.h"
#include "NETComponentClass.h"

/*
"name":"MyComponent",
         "fields":[
            {
               "isEnum":false,
               "typeName":"Boolean",
               "name":"MyBoolValue",
               "defaultValue":"1",
               "caPos":[
                  "",
                  "",
                  ""
               ],
               "caNamed":{

               }
            },
*/
namespace Atomic
{

HashMap<String, VariantType> NETComponentClass::typeMap_;

NETComponentClass::NETComponentClass(Context* context, NETAssemblyFile* assemblyFile) :
    Object(context),
    assemblyFile_(assemblyFile)
{

}

NETComponentClass::~NETComponentClass()
{

}

void NETComponentClass::InitTypeMap()
{
    typeMap_["Boolean"] = VAR_BOOL;
    typeMap_["Int32"] = VAR_INT;
    typeMap_["Single"] = VAR_FLOAT;
    typeMap_["Double"] = VAR_DOUBLE;
    typeMap_["String"] = VAR_STRING;
    typeMap_["Vector2"] = VAR_VECTOR2;
    typeMap_["Vector3"] = VAR_VECTOR3;
    typeMap_["Vector4"] = VAR_VECTOR4;
    typeMap_["Quaternion"] = VAR_QUATERNION;

    // maps enum types
    typeMap_["Enum"] = VAR_INT;

}

void NETComponentClass::GetDefaultFieldValue(const String& name, Variant& v)
{

    // This is duplicated in JSComponentFile
    v = Variant::EMPTY;

    VariantMap::ConstIterator itr = defaultFieldValues_.Find(name);

    if (itr == defaultFieldValues_.End())
    {
        HashMap<String, VariantType>::ConstIterator citr = fields_.Find(name);
        if (citr == fields_.End())
            return;

        switch (citr->second_)
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

        return;

    }

    v = itr->second_;
}

bool NETComponentClass::ParseJSON(const JSONValue& json)
{
    if (!typeMap_.Size())
        InitTypeMap();

    const HashMap<String, Vector<EnumInfo>>& fileEnums = assemblyFile_->GetEnums();

    enums_.Clear();

    name_ = json.Get("name").GetString();

    const JSONValue& jfields = json.Get("fields");

    if (jfields.IsArray())
    {
        for (unsigned i = 0; i < jfields.GetArray().Size(); i++)
        {
            const JSONValue& jfield = jfields.GetArray().At(i);

            VariantType varType = VAR_NONE;

            bool isEnum = jfield.Get("isEnum").GetBool();
            String typeName = jfield.Get("typeName").GetString();
            String fieldName = jfield.Get("name").GetString();
            String defaultValue = jfield.Get("defaultValue").GetString();

            if (isEnum)
            {
                if (fileEnums.Contains(typeName))
                {
                    Vector<EnumInfo> values;

                    for (unsigned i = 0; i < fileEnums[typeName]->Size(); i++)
                    {
                        values.Push(fileEnums[typeName]->At(i));
                    }

                    enums_[fieldName] = values;

                    varType = typeMap_["Enum"];

                    fields_[fieldName] = varType;

                    Variant value;
                    value.FromString(varType, defaultValue);
                    defaultFieldValues_[fieldName] = value;
                }

            }
            else
            {
                if (!typeMap_.Contains(typeName))
                {
                    LOGERRORF("Unknown NETComponentClass Field type: %s", typeName.CString());
                    continue;
                }

                varType = typeMap_[typeName];

                fields_[fieldName] = varType;

                Variant value;
                value.FromString(varType, defaultValue);
                defaultFieldValues_[fieldName] = value;
            }

        }

    }

    return true;
}

}
