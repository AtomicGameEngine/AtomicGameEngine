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

#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/SceneEvents.h>

#include "NETScriptEvents.h"
#include "CSComponent.h"

namespace Atomic
{

extern const char* LOGIC_CATEGORY;

CSComponent::CSComponent(Context* context) :
    ScriptComponent(context)
{
    
}

CSComponent::~CSComponent()
{

}

void CSComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<CSComponent>(LOGIC_CATEGORY);

    ATOMIC_ACCESSOR_ATTRIBUTE("Class", GetComponentClassName, SetComponentClassName, String, String::EMPTY, AM_DEFAULT);
    ATOMIC_COPY_BASE_ATTRIBUTES(ScriptComponent);

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
}

void CSComponent::SetComponentClassName(const String& name)
{
    if (componentClassName_ == name)
        return;

    componentClassName_ = name;

    if (context_->GetEditorContext())
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

}

void CSComponent::OnSceneSet(Scene* scene)
{

}

void CSComponent::SendLoadEvent()
{
    if (!componentClassName_.Length())
        return;

    using namespace CSComponentLoad;

    VariantMap eventData;

    eventData[P_CLASSNAME] = componentClassName_;
    eventData[P_NATIVEINSTANCE] = (void*) this;

    if (!fieldValues_.Empty())
        eventData[P_FIELDVALUES] = (void*) &fieldValues_;

    if (node_ && node_->GetScene())
        node_->GetScene()->SendEvent(E_CSCOMPONENTLOAD, eventData);

}

bool CSComponent::Load(Deserializer& source, bool setInstanceDefault)
{
    bool success = ScriptComponent::Load(source, setInstanceDefault);

    if (success)
        SendLoadEvent();

    return success;
}

bool CSComponent::LoadXML(const XMLElement& source, bool setInstanceDefault)
{
    bool success = ScriptComponent::LoadXML(source, setInstanceDefault);

    if (success)
        SendLoadEvent();

    return success;
}

ScriptComponentFile* CSComponent::GetComponentFile() const
{
    if (!componentClassName_.Length())
        return 0;

    return CSComponentAssembly::ResolveClassAssembly(componentClassName_);

}


}
