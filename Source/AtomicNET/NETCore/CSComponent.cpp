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

#include "CSComponent.h"
//#include "AtomicSharpAPI.h"

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

        if (source != XMLElement::EMPTY)
        {
            XMLElement attrElem = source.GetChild("attribute");

            while (attrElem)
            {
                if (attrElem.GetAttribute("name") == "ManagedClass")
                {
                    managedClass = attrElem.GetAttribute("value");
                    break;
                }

                attrElem = attrElem.GetNext("attribute");
            }
        }

        SharedPtr<Object> ptr;

        if (managedClass.Length())
        {
            // change to callback
            //ptr = CSComponentCreate(managedClass);
        }

        if (ptr.Null())
        {
            ptr = new CSComponent(context_);
        }

        return ptr;    }
};


CSComponent::CSComponent(Context* context) :
    Component(context),
    updateEventMask_(USE_UPDATE | USE_POSTUPDATE | USE_FIXEDUPDATE | USE_FIXEDPOSTUPDATE),
    currentEventMask_(0),
    instanceInitialized_(false),
    started_(false),
    destroyed_(false),
    scriptClassInstance_(false),
    delayedStartCalled_(false),
    loading_(false)
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

    MIXED_ACCESSOR_ATTRIBUTE("Assembly", GetAssemblyFileAttr, SetAssemblyFileAttr, ResourceRef, ResourceRef(NETAssemblyFile::GetTypeStatic()), AM_DEFAULT);

    ACCESSOR_ATTRIBUTE("Class", GetComponentClassName, SetComponentClassName, String, String::EMPTY, AM_DEFAULT);

}

void CSComponent::OnSetEnabled()
{
    UpdateEventSubscription();
}

void CSComponent::SetUpdateEventMask(unsigned char mask)
{
    if (updateEventMask_ != mask)
    {
        updateEventMask_ = mask;
        UpdateEventSubscription();
    }
}

void CSComponent::ApplyAttributes()
{
}

void CSComponent::CallScriptMethod(CSComponentMethod method, float value)
{
    if (destroyed_ || !node_ || !node_->GetScene())
        return;

    // Change to callback
    //CSComponentCallMethod(GetRefID(), method, value);

}

void CSComponent::Start()
{
    CallScriptMethod(CSComponentMethod_Start);
}

void CSComponent::DelayedStart()
{
    CallScriptMethod(CSComponentMethod_DelayedStart);
}

void CSComponent::Update(float timeStep)
{
    //if (!instanceInitialized_)
    //    InitInstance();

    if (!started_)
    {
        started_ = true;
        Start();
    }

    CallScriptMethod(CSComponentMethod_Update, timeStep);
}

void CSComponent::PostUpdate(float timeStep)
{
    CallScriptMethod(CSComponentMethod_PostUpdate, timeStep);
}

void CSComponent::FixedUpdate(float timeStep)
{
    CallScriptMethod(CSComponentMethod_FixedUpdate, timeStep);
}

void CSComponent::FixedPostUpdate(float timeStep)
{
    CallScriptMethod(CSComponentMethod_PostFixedUpdate, timeStep);
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
        Stop();
    }
}

void CSComponent::OnSceneSet(Scene* scene)
{
    if (scene)
        UpdateEventSubscription();
    else
    {
        UnsubscribeFromEvent(E_SCENEUPDATE);
        UnsubscribeFromEvent(E_SCENEPOSTUPDATE);
#ifdef ATOMIC_PHYSICS
        UnsubscribeFromEvent(E_PHYSICSPRESTEP);
        UnsubscribeFromEvent(E_PHYSICSPOSTSTEP);
#endif
        currentEventMask_ = 0;
    }
}

void CSComponent::UpdateEventSubscription()
{
    Scene* scene = GetScene();
    if (!scene)
        return;

    bool enabled = IsEnabledEffective();

    bool needUpdate = enabled && ((updateEventMask_ & USE_UPDATE) || !delayedStartCalled_);
    if (needUpdate && !(currentEventMask_ & USE_UPDATE))
    {
        SubscribeToEvent(scene, E_SCENEUPDATE, HANDLER(CSComponent, HandleSceneUpdate));
        currentEventMask_ |= USE_UPDATE;
    }
    else if (!needUpdate && (currentEventMask_ & USE_UPDATE))
    {
        UnsubscribeFromEvent(scene, E_SCENEUPDATE);
        currentEventMask_ &= ~USE_UPDATE;
    }

    bool needPostUpdate = enabled && (updateEventMask_ & USE_POSTUPDATE);
    if (needPostUpdate && !(currentEventMask_ & USE_POSTUPDATE))
    {
        SubscribeToEvent(scene, E_SCENEPOSTUPDATE, HANDLER(CSComponent, HandleScenePostUpdate));
        currentEventMask_ |= USE_POSTUPDATE;
    }
    else if (!needUpdate && (currentEventMask_ & USE_POSTUPDATE))
    {
        UnsubscribeFromEvent(scene, E_SCENEPOSTUPDATE);
        currentEventMask_ &= ~USE_POSTUPDATE;
    }

#ifdef ATOMIC_PHYSICS
    PhysicsWorld* world = scene->GetComponent<PhysicsWorld>();
    if (!world)
        return;

    bool needFixedUpdate = enabled && (updateEventMask_ & USE_FIXEDUPDATE);
    if (needFixedUpdate && !(currentEventMask_ & USE_FIXEDUPDATE))
    {
        SubscribeToEvent(world, E_PHYSICSPRESTEP, HANDLER(CSComponent, HandlePhysicsPreStep));
        currentEventMask_ |= USE_FIXEDUPDATE;
    }
    else if (!needFixedUpdate && (currentEventMask_ & USE_FIXEDUPDATE))
    {
        UnsubscribeFromEvent(world, E_PHYSICSPRESTEP);
        currentEventMask_ &= ~USE_FIXEDUPDATE;
    }

    bool needFixedPostUpdate = enabled && (updateEventMask_ & USE_FIXEDPOSTUPDATE);
    if (needFixedPostUpdate && !(currentEventMask_ & USE_FIXEDPOSTUPDATE))
    {
        SubscribeToEvent(world, E_PHYSICSPOSTSTEP, HANDLER(CSComponent, HandlePhysicsPostStep));
        currentEventMask_ |= USE_FIXEDPOSTUPDATE;
    }
    else if (!needFixedPostUpdate && (currentEventMask_ & USE_FIXEDPOSTUPDATE))
    {
        UnsubscribeFromEvent(world, E_PHYSICSPOSTSTEP);
        currentEventMask_ &= ~USE_FIXEDPOSTUPDATE;
    }
#endif
}

void CSComponent::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace SceneUpdate;

    assert(!destroyed_);

    // Execute user-defined delayed start function before first update
    if (!delayedStartCalled_)
    {
        DelayedStart();
        delayedStartCalled_ = true;

        // If did not need actual update events, unsubscribe now
        if (!(updateEventMask_ & USE_UPDATE))
        {
            UnsubscribeFromEvent(GetScene(), E_SCENEUPDATE);
            currentEventMask_ &= ~USE_UPDATE;
            return;
        }
    }

    // Then execute user-defined update function
    Update(eventData[P_TIMESTEP].GetFloat());
}

void CSComponent::HandleScenePostUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace ScenePostUpdate;

    // Execute user-defined post-update function
    PostUpdate(eventData[P_TIMESTEP].GetFloat());
}

#ifdef ATOMIC_PHYSICS
void CSComponent::HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData)
{
    using namespace PhysicsPreStep;

    // Execute user-defined fixed update function
    FixedUpdate(eventData[P_TIMESTEP].GetFloat());
}

void CSComponent::HandlePhysicsPostStep(StringHash eventType, VariantMap& eventData)
{
    using namespace PhysicsPostStep;

    // Execute user-defined fixed post-update function
    FixedPostUpdate(eventData[P_TIMESTEP].GetFloat());
}
#endif

bool CSComponent::Load(Deserializer& source, bool setInstanceDefault)
{
    loading_ = true;
    bool success = Component::Load(source, setInstanceDefault);
    loading_ = false;

    return success;
}

bool CSComponent::LoadXML(const XMLElement& source, bool setInstanceDefault)
{
    loading_ = true;
    bool success = Component::LoadXML(source, setInstanceDefault);
    loading_ = false;

    return success;
}

void CSComponent::SetAssemblyFile(NETAssemblyFile* assemblyFile)
{
    assemblyFile_ = assemblyFile;
}

ResourceRef CSComponent::GetAssemblyFileAttr() const
{
    return GetResourceRef(assemblyFile_, NETAssemblyFile::GetTypeStatic());
}

void CSComponent::SetAssemblyFileAttr(const ResourceRef& value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetAssemblyFile(cache->GetResource<NETAssemblyFile>(value.name_));
}


}
