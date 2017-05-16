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

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Profiler.h>
#include <Atomic/IO/Deserializer.h>
#include <Atomic/IO/Serializer.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/JSONFile.h>
#include <Atomic/Resource/ResourceMapRouter.h>
#include <Atomic/Script/ScriptSystem.h>

#include "NETScriptEvents.h"
#include "CSComponentAssembly.h"

namespace Atomic
{

    HashMap<StringHash, VariantType> CSComponentAssembly::typeMap_;

    CSComponentAssembly::CSComponentAssembly(Context* context) :
        ScriptComponentFile(context)
    {

    }

    CSComponentAssembly::~CSComponentAssembly()
    {

    }

    void CSComponentAssembly::InitTypeMap()
    {
        typeMap_["Boolean"] = VAR_BOOL;
        typeMap_["Int32"] = VAR_INT;
        typeMap_["UInt32"] = VAR_INT;
        typeMap_["Single"] = VAR_FLOAT;
        typeMap_["Double"] = VAR_DOUBLE;
        typeMap_["String"] = VAR_STRING;
        typeMap_["Vector2"] = VAR_VECTOR2;
        typeMap_["Vector3"] = VAR_VECTOR3;
        typeMap_["Vector4"] = VAR_VECTOR4;
        typeMap_["Quaternion"] = VAR_QUATERNION;
        typeMap_["IntVector2"] = VAR_INTVECTOR2;
        typeMap_["Color"] = VAR_COLOR;

    }

    CSComponent* CSComponentAssembly::CreateCSComponent(const String& classname)
    {
        return nullptr;
    }

    bool CSComponentAssembly::ParseComponentClassJSON(const JSONValue& json)
    {
        if (!typeMap_.Size())
            InitTypeMap();

        String className = json.Get("name").GetString();

        classNames_.Push(className);

        const JSONValue& jfields = json.Get("fields");

        PODVector<StringHash> enumsAdded;

        if (jfields.IsArray())
        {
            for (unsigned i = 0; i < jfields.GetArray().Size(); i++)
            {
                const JSONValue& jfield = jfields.GetArray().At(i);

                VariantType varType = VAR_NONE;

                bool isEnum = jfield.Get("isEnum").GetBool();
                bool isArray = jfield.Get("isArray").GetBool();
                unsigned fixedArraySize = 0;
                String typeName = jfield.Get("typeName").GetString();
                String resourceTypeName;
                String fieldName = jfield.Get("name").GetString();
                String defaultValue = jfield.Get("defaultValue").GetString();
                String tooltip;

                if (!defaultValue.Length())
                {
                    JSONArray caPos = jfield.Get("caPos").GetArray();
                    if (caPos.Size())
                        defaultValue = caPos[0].GetString();
                }

                JSONObject caNamed = jfield.Get("caNamed").GetObject();

                if (!defaultValue.Length())
                {                    
                    if (caNamed.Contains("DefaultValue"))
                        defaultValue = caNamed["DefaultValue"].GetString();
                }

                // tooltip
                if (caNamed.Contains("Tooltip"))
                {
                    tooltip = caNamed["Tooltip"].GetString();
                }

                // fixed array size
                if (caNamed.Contains("ArraySize"))
                {
                    fixedArraySize = (unsigned)ToInt(caNamed["ArraySize"].GetString().CString());
                }


                if (isEnum && assemblyEnums_.Contains(typeName) && !enumsAdded.Contains(fieldName))
                {
                    varType = VAR_INT;
                    enumsAdded.Push(fieldName);
                    const Vector<EnumInfo>& einfos = assemblyEnums_[typeName];
                    for (unsigned i = 0; i < einfos.Size(); i++)
                        AddEnum(/*typeName*/fieldName, einfos[i], className);
                }

                if (varType == VAR_NONE && typeMap_.Contains(typeName))
                    varType = typeMap_[typeName];

                if (varType == VAR_NONE)
                {
                    // FIXME: We need to be able to test if a type is a ResourceRef, this isn't really the way to achieve that
                    const HashMap<StringHash, SharedPtr<ObjectFactory>>& factories = context_->GetObjectFactories();
                    HashMap<StringHash, SharedPtr<ObjectFactory>>::ConstIterator itr = factories.Begin();

                    while (itr != factories.End())
                    {
                        if (itr->second_->GetFactoryTypeName() == typeName)
                        {
                            varType = VAR_RESOURCEREF;
                            resourceTypeName = typeName;
                            break;
                        }

                        itr++;
                    }

                    if (varType == VAR_NONE)
                    {
                        ATOMIC_LOGERRORF("Component Class %s contains unmappable type %s in field %s",
                            className.CString(), typeName.CString(), fieldName.CString());

                        continue;
                    }

                }

                if (!isArray)
                {
                    if (!defaultValue.Length() && varType == VAR_RESOURCEREF)
                    {
                        // We still need a default value for ResourceRef's so we know the classtype
                        AddDefaultValue(fieldName, ResourceRef(typeName), className);
                    }
                    else
                    {
                        Variant value;

                        if (varType == VAR_RESOURCEREF)
                        {
                            ResourceRef rref(typeName);
                            rref.name_ = defaultValue;
                            value = rref;
                        }
                        else
                        {
                            value.FromString(varType, defaultValue);
                        }

                        AddDefaultValue(fieldName, value, className);
                    }
                }

                AddField(fieldName, varType, resourceTypeName, isArray, isEnum, fixedArraySize, className, tooltip);

            }

        }

        return true;
    }

    bool CSComponentAssembly::ParseAssemblyJSON(const JSONValue& json)
    {
        Clear();
        assemblyEnums_.Clear();
        classNames_.Clear();

        const JSONArray& enums = json.Get("enums").GetArray();

        // parse to all enums hash
        for (unsigned i = 0; i < enums.Size(); i++)
        {
            const JSONValue& ejson = enums.At(i);

            String enumName = ejson.Get("name").GetString();

            const JSONObject& evalues = ejson.Get("values").GetObject();

            JSONObject::ConstIterator itr = evalues.Begin();

            Vector<EnumInfo> values;

            while (itr != evalues.End())
            {
                EnumInfo info;
                info.name_ = itr->first_;
                info.value_ = itr->second_.GetInt();
                values.Push(info);
                itr++;
            }

            assemblyEnums_[enumName] = values;
        }

        const JSONArray& components = json.Get("components").GetArray();

        for (unsigned i = 0; i < components.Size(); i++)
        {
            const JSONValue& cjson = components.At(i);

            ParseComponentClassJSON(cjson);
        }

        return true;
    }

    void CSComponentAssembly::RegisterObject(Context* context)
    {
        context->RegisterFactory<CSComponentAssembly>();
    }

    bool CSComponentAssembly::BeginLoad(Deserializer& source)
    {
        // TODO: Assemblies in packages?
        File* sourceFile = (File*) &source;

        fullAssemblyPath_ = sourceFile->GetFullPath();

        String text = sourceFile->ReadText();
        JSONValue root;

        if (!JSONFile::ParseJSON(text, root, false))        
        {
            ATOMIC_LOGERRORF("Failed to load assembly json for %s ", fullAssemblyPath_.CString());
            return false;
        }

        if (!ParseAssemblyJSON(root))
        {
            ATOMIC_LOGERRORF("Failed to parse assembly json for %s ", fullAssemblyPath_.CString());
            return false;
        }

        VariantMap eventData;

        using namespace CSComponentAssemblyReference;
        eventData[P_ASSEMBLYPATH] = fullAssemblyPath_;

        SendEvent(E_CSCOMPONENTASSEMBLYREFERENCE, eventData);

        return true;
    }

    bool CSComponentAssembly::Save(Serializer& dest) const
    {
        return true;
    }

    CSComponentAssembly* CSComponentAssembly::ResolveClassAssembly(const String& fullClassName)
    {
        Context* context = ScriptSystem::GetContext();
        assert(context);

        String classname = fullClassName;
        String csnamespace;

        // Handle namespaces
        if (fullClassName.Contains('.'))
        {

            StringVector elements = fullClassName.Split('.');

            if (elements.Size() <= 1)
                return 0;

            classname = elements.Back();
            elements.Pop();

            csnamespace = String::Joined(elements, ".");
        }

        ResourceCache* cache = context->GetSubsystem<ResourceCache>();

        PODVector<CSComponentAssembly*> assemblies;

        cache->GetResources<CSComponentAssembly>(assemblies);

        for (unsigned i = 0; i < assemblies.Size(); i++)
        {
            CSComponentAssembly* assembly = assemblies[i];

            // TODO: support namespaces
            const StringVector& classNames = assembly->GetClassNames();
            if (classNames.Contains(classname))
            {
                return assembly;
            }

        }

        return 0;

    }

    bool CSComponentAssembly::PreloadClassAssemblies()
    {
        ATOMIC_LOGINFO("Preloading Class Assemblies");

        Context* context = ScriptSystem::GetContext();
        
        if (!context)
            return false;

        ResourceMapRouter* resourceMap = context->GetSubsystem<ResourceMapRouter>();

        if (!resourceMap)
        {
            ATOMIC_LOGERROR("CSComponentAssembly::PreloadClassAssemblies - ResourceMapRouter subsystem unavailable");
            return false;
        }

        ResourceCache* cache = context->GetSubsystem<ResourceCache>();

        if (!cache)
        {
            ATOMIC_LOGERROR("CSComponentAssembly::PreloadClassAssemblies - ResourceCache subsystem unavailable");
            return false;
        }

        const HashMap<StringHash, String>* assemblies = resourceMap->GetTypeCacheMap("CSComponentAssembly");

        if (!assemblies)
            return true;

        HashMap<StringHash, String>::ConstIterator itr = assemblies->Begin();

        while (itr != assemblies->End())
        {
            if (!cache->GetResource<CSComponentAssembly>(itr->second_, false))
            {
                ATOMIC_LOGERRORF("CSComponentAssembly::PreloadClassAssemblies - Error getting resource %s", itr->second_.CString());
                return false;
            }

            itr++;
        }

        return true;

    }

}
