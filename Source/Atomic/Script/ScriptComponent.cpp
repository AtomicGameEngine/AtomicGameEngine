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


#include "../Core/Context.h"
#include "../IO/Log.h"

#include "ScriptComponentFile.h"
#include "ScriptComponent.h"

namespace Atomic
{

ScriptComponent::ScriptComponent(Context* context) : Component(context),
    saving_(false),
    loading_(false)

{

}

void ScriptComponent::RegisterObject(Context* context)
{
    ATOMIC_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
    ATOMIC_ACCESSOR_ATTRIBUTE("FieldValues", GetFieldValuesAttr, SetFieldValuesAttr, VariantMap, Variant::emptyVariantMap, AM_FILE);
}

ScriptComponent::~ScriptComponent()
{

}

bool ScriptComponent::Load(Deserializer& source, bool setInstanceDefault)
{
    loading_ = true;
    bool success = Component::Load(source, setInstanceDefault);
    loading_ = false;

    return success;
}

bool ScriptComponent::LoadXML(const XMLElement& source, bool setInstanceDefault)
{
    loading_ = true;
    bool success = Component::LoadXML(source, setInstanceDefault);
    loading_ = false;

    return success;
}

bool ScriptComponent::Save(Serializer& dest) const
{
    saving_ = true;    
    bool success = Component::Save(dest);    
    saving_ = false;
    
    return success;
}

bool ScriptComponent::SaveXML(XMLElement& dest) const
{
    saving_ = true;
    bool success = Component::SaveXML(dest);
    saving_ = false;

    return success;
}

const VariantMap& ScriptComponent::GetFieldValuesAttr() const
{
    ScriptComponentFile* componentFile = GetComponentFile();
    const String& classname = GetComponentClassName();

    if (saving_ && componentFile && classname.Length())
    {
        // When serializing enumerations, enum integer values are converted to enum strings
        // This makes them independent of enum values/order changing

        fieldValuesAttr_ = fieldValues_;

        const EnumMap& enums = componentFile->GetEnums(classname);
        VariantMap::ConstIterator itr = fieldValues_.Begin();
        String fieldName;

        while (itr != fieldValues_.End())
        {
            const StringHash& nameHash = itr->first_;
            const Variant& v = itr->second_;
            const FieldInfo* fieldInfo = 0;

            bool skip = false;

            // If we're not a numeric field, can skip
            if (v.GetType() != VAR_INT &&
                v.GetType() != VAR_FLOAT &&
                v.GetType() != VAR_DOUBLE)
            {
                skip = true;
            }
            else if (!StringHash::GetSignificantString(nameHash, fieldName))
            {
                ATOMIC_LOGWARNING("ScriptComponent::GetFieldValuesAttr - unable to retrieve field name");
                skip = true;
            }
            else if (!(fieldInfo = componentFile->GetFields(classname)[fieldName]) || !fieldInfo->isEnum_)
            {
                skip = true;
            }

            if (skip)
            {
                itr++;
                continue;
            }
           
            int intValue = v.GetInt();

            EnumMap::ConstIterator eitr = enums.Begin();

            // find corresponding enum, and convert to enum name representation
            while (eitr != enums.End())
            {
                if (!eitr->first_.Compare(fieldName, false))
                {
                    const Vector<EnumInfo>& infos = eitr->second_;

                    for (unsigned i = 0; i < infos.Size(); i++)
                    {
                        if (intValue == infos[i].value_)
                        {
                            // use enum name string instead of numeric value
                            fieldValuesAttr_[nameHash] = infos[i].name_;
                            break;
                        }
                    }

                    break;
                }

                eitr++;
            }

            itr++;
        }

        return fieldValuesAttr_;
    }

    return fieldValues_;
}

void ScriptComponent::SetFieldValuesAttr(const VariantMap& value)
{
    fieldValues_ = value;

    ScriptComponentFile* componentFile = GetComponentFile();
    const String& classname = GetComponentClassName();

    if (loading_ && componentFile && classname.Length())
    {
        // When serializing enumerations, enum integer values are converted to enum strings
        // This makes them independent of enum values/order changing

        const EnumMap& enums = componentFile->GetEnums(classname);
        VariantMap::ConstIterator itr = value.Begin();
        String fieldName;

        // run through field values looking for enum name strings
        // which need to be converted from string to numeric representation
        while (itr != value.End())
        {
            const StringHash& nameHash = itr->first_;
            const FieldInfo* fieldInfo = 0;

            bool skip = false;

            if (itr->second_.GetType() != VAR_STRING)
            {
                skip = true;
            }
            else if (!StringHash::GetSignificantString(nameHash, fieldName))
            {
                ATOMIC_LOGWARNING("ScriptComponent::SetFieldValuesAttr - unable to retrieve field name");
                skip = true;
            }
            else if (!(fieldInfo = componentFile->GetFields(classname)[fieldName]) || !fieldInfo->isEnum_)
            {
                skip = true;
            }

            if (skip)
            {
                itr++;
                continue;
            }            

            EnumMap::ConstIterator eitr = enums.Begin();
            const String& svalue = itr->second_.GetString();

            // find the corresponding enum if any
            while (eitr != enums.End())
            {
                if (!eitr->first_.Compare(fieldName, false))
                {
                    const Vector<EnumInfo>& infos = eitr->second_;

                    // default
                    fieldValues_[nameHash] = infos.Size() > 0 ? infos[0].value_ : 0;

                    for (unsigned i = 0; i < infos.Size(); i++)
                    {
                        if (!svalue.Compare(infos[i].name_, false))
                        {
                            fieldValues_[nameHash] = infos[i].value_;
                            break;
                        }
                    }

                    break;
                }

                eitr++;
            }

            itr++;
        }

    }

}

}
