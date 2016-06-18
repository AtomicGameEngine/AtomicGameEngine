//
// Copyright (c) 2008-2014 the Urho3D project.
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
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Core/Context.h>
#include <Atomic/Resource/ResourceCache.h>

#ifdef ATOMIC_PHYSICS
#include <Atomic/Physics/PhysicsEvents.h>
#include <Atomic/Physics/PhysicsWorld.h>
#endif
#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/SceneEvents.h>

#include "../NETCore/NETVariant.h"
#include "NETScriptEvents.h"
#include "CSManaged.h"
#include "CSComponent.h"

namespace Atomic
{

extern const char* LOGIC_CATEGORY;

class CSComponentFactory : public ObjectFactory
{
public:
    /// Construct.
    CSComponentFactory(Context* context) :
        ObjectFactory(context)
    {
        type_ = CSComponent::GetTypeStatic();
        baseType_ = CSComponent::GetBaseTypeStatic();
        typeName_ = CSComponent::GetTypeNameStatic();
    }

    /// Create an object of the specific type.
    SharedPtr<Object> CreateObject(const XMLElement& source = XMLElement::EMPTY)
    {

        // if in editor, just create the CSComponent
        if (context_->GetEditorContext())
        {
            return SharedPtr<Object>(new CSComponent(context_));
        }

        // At runtime, a XML CSComponent may refer to a managed component

        String managedClass;
        String assemblyRef;

        if (source != XMLElement::EMPTY)
        {
            XMLElement attrElem = source.GetChild("attribute");

            while (attrElem)
            {
                if (attrElem.GetAttribute("name") == "Assembly")
                {
                    assemblyRef = attrElem.GetAttribute("value");
                }
                else if (attrElem.GetAttribute("name") == "Class")
                {
                    managedClass = attrElem.GetAttribute("value");
                }

                if (assemblyRef.Length() && managedClass.Length())
                    break;

                attrElem = attrElem.GetNext("attribute");
            }
        }               

        SharedPtr<Object> ptr;

        if (assemblyRef.Length())
        {
            Vector<String> split = assemblyRef.Split(';');

            if (split.Size() == 2)
            {
                ResourceCache* cache = context_->GetSubsystem<ResourceCache>();
                CSComponentAssembly* componentFile = cache->GetResource<CSComponentAssembly>(split[1]);
                if (componentFile)
                    ptr = componentFile->CreateCSComponent(managedClass);
                else
                {
                    LOGERRORF("Unable to load component file %s", split[1].CString());
                }
            }

        }

        if (ptr.Null())
        {
            ptr = new CSComponent(context_);
        }

        return ptr;    }
};


CSComponent::CSComponent(Context* context) :
    ScriptComponent(context)
{

}

CSComponent::~CSComponent()
{

}

void CSComponent::RegisterObject(Context* context)
{
    context->RegisterFactory(new CSComponentFactory(context), LOGIC_CATEGORY);
    ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);

    ATTRIBUTE("FieldValues", VariantMap, fieldValues_, Variant::emptyVariantMap, AM_FILE);

    MIXED_ACCESSOR_ATTRIBUTE("Assembly", GetAssemblyFileAttr, SetAssemblyFileAttr, ResourceRef, ResourceRef(CSComponentAssembly::GetTypeStatic()), AM_DEFAULT);

    ACCESSOR_ATTRIBUTE("Class", GetComponentClassName, SetComponentClassName, String, String::EMPTY, AM_DEFAULT);

}

void CSComponent::OnSetEnabled()
{
}

void CSComponent::ApplyAttributes()
{
}

void CSComponent::ApplyFieldValues()
{
    if (!fieldValues_.Size())
        return;

    SharedPtr<NETVariantMap> vmap(new NETVariantMap());
    vmap->CopySourceVariantMap(fieldValues_);

    CSManaged* managed = GetSubsystem<CSManaged>();

    managed->CSComponentApplyFields(this, vmap);

}

void CSComponent::SetComponentClassName(const String& name)
{
    componentClassName_ = name;

    if (assemblyFile_ && assemblyFile_->GetClassNames().Contains(name))
    {
        using namespace CSComponentClassChanged;
        VariantMap eventData;
        eventData[P_CSCOMPONENT] = this;
        eventData[P_CLASSNAME] = name;
        SendEvent(E_CSCOMPONENTCLASSCHANGED, eventData);
    }
}

void CSComponent::OnNodeSet(Node* node)
{
    if (node)
    {
        //UpdateReferences();
    }
    else
    {
        // We are being detached from a node: execute user-defined stop function and prepare for destruction
        //UpdateReferences(true);
        //Stop();
    }
}

void CSComponent::OnSceneSet(Scene* scene)
{
}

bool CSComponent::Load(Deserializer& source, bool setInstanceDefault)
{
    bool success = Component::Load(source, setInstanceDefault);
    return success;
}

bool CSComponent::LoadXML(const XMLElement& source, bool setInstanceDefault)
{
    bool success = Component::LoadXML(source, setInstanceDefault);
    return success;
}

void CSComponent::SetAssemblyFile(CSComponentAssembly* assemblyFile)
{
    assemblyFile_ = assemblyFile;
}

ResourceRef CSComponent::GetAssemblyFileAttr() const
{
    return GetResourceRef(assemblyFile_, CSComponentAssembly::GetTypeStatic());
}

void CSComponent::SetAssemblyFileAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetAssemblyFile(cache->GetResource<CSComponentAssembly>(value.name_));
}


}
