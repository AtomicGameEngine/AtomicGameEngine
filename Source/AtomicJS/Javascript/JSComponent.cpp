// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine


#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#ifdef ATOMIC_PHYSICS
#include <Atomic/Physics/PhysicsEvents.h>
#include <Atomic/Physics/PhysicsWorld.h>
#endif

#include <Atomic/Core/Profiler.h>
#include <Atomic/IO/MemoryBuffer.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Resource/ResourceEvents.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/SceneEvents.h>
#include <Atomic/Atomic2D/PhysicsEvents2D.h>
#include <Atomic/Atomic2D/PhysicsWorld2D.h>
#include <Atomic/Atomic2D/RigidBody2D.h>
#include <Atomic/UI/UIEvents.h>

#include "Javascript.h"
#include "JSEvents.h"
#include "JSComponent.h"
#include "JSAPI.h"

namespace Atomic
{

static const char* methodDeclarations[] = {
    "start",
    "stop",
    "delayedStart",
    "update",
    "postUpdate",
    "fixedUpdate",
    "fixedPostUpdate",
    "load",
    "save",
    "readNetworkUpdate",
    "writeNetworkUpdate",
    "applyAttributes",
    "transformChanged"
};


extern const char* LOGIC_CATEGORY;

JSComponent::JSComponent(Context* context) :
    Component(context),
    script_(GetSubsystem<Javascript>()),
    scriptObject_(0),
    subscribed_(false),
    subscribedPostFixed_(false),
    started_(false),
    destroyed_(false)
{
    vm_ = JSVM::GetJSVM(NULL);
    ClearScriptMethods();
}

JSComponent::~JSComponent()
{
}

void JSComponent::OnNodeSet(Node *node)
{
    Component::OnNodeSet(node);

    if (node && node->JSGetHeapPtr())
    {
        //assert(node->JSGetHeapPtr());

        duk_context* ctx = vm_->GetJSContext();
        int top = duk_get_top(ctx);
        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, JS_GLOBALSTASH_INDEX_NODE_REGISTRY);
        // can't use instance as key, as this coerces to [Object] for
        // string property, pointer will be string representation of
        // address, so, unique key
        duk_push_pointer(ctx, (void*) node);
        js_push_class_object_instance(ctx, node);
        duk_put_prop(ctx, -3);
        duk_pop_2(ctx);
        assert(duk_get_top(ctx) == top);
    }
}

void JSComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<JSComponent>(LOGIC_CATEGORY);
    ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
    ACCESSOR_ATTRIBUTE("Component Name", GetClassNameProperty, SetClassNameProperty, String, String::EMPTY, AM_DEFAULT);
}

void JSComponent::ApplyAttributes()
{
    Component::ApplyAttributes();

    if (classNameProperty_.Length())
        SetClassName(classNameProperty_);

}

void JSComponent::ClearScriptMethods()
{
    for (unsigned i = 0; i < MAX_JSSCRIPT_METHODS; ++i)
        methods_[i] = 0;

    //delayedCalls_.Clear();
}

void JSComponent::OnSetEnabled()
{
    UpdateEventSubscription();
}

void JSComponent::ListenToEvent(Object* sender, StringHash eventType, JS_HEAP_PTR __duk_function)
{
    duk_context* ctx = vm_->GetJSContext();
    duk_push_heapptr(ctx, __duk_function);
    assert(duk_is_function(ctx, -1));
    duk_pop(ctx);

    scriptEventFunctions_[eventType] = __duk_function;
    if (sender)
        SubscribeToEvent(sender, eventType, HANDLER(JSComponent, HandleScriptEvent));
    else
        SubscribeToEvent(eventType, HANDLER(JSComponent, HandleScriptEvent));

}

bool JSComponent::CreateObject(JSFile* scriptFile, const String& className)
{
    className_ = String::EMPTY; // Do not create object during SetScriptFile()
    SetScriptFile(scriptFile);
    SetClassName(className);
    return scriptObject_ != 0;
}

void JSComponent::SetClassName(const String& className)
{
    assert(className.Length());

    if (className == className_ && scriptObject_)
        return;

    ReleaseObject();

    className_ = className;
    CreateObject();
    MarkNetworkUpdate();
}


void JSComponent::ReleaseObject()
{
    if (scriptObject_)
    {
        //if (methods_[JSMETHOD_STOP])
        //    scriptFile_->Execute(scriptObject_, methods_[JSMETHOD_STOP]);

        PODVector<StringHash> exceptions;
        exceptions.Push(E_RELOADSTARTED);
        exceptions.Push(E_RELOADFINISHED);
        UnsubscribeFromAllEventsExcept(exceptions, false);

        if (node_)
            node_->RemoveListener(this);

        subscribed_ = false;
        subscribedPostFixed_ = false;

        ClearScriptMethods();

        scriptObject_ = 0;
    }
}


void JSComponent::SetScriptFile(JSFile* scriptFile)
{

    ReleaseObject();

    CreateObject();
    MarkNetworkUpdate();
}

void JSComponent::CreateObject()
{
    if (className_.Empty())
        return;

    PROFILE(CreateScriptObject);

    duk_context* ctx = vm_->GetJSContext();

    duk_push_global_stash(ctx);
    duk_get_prop_index(ctx, -1, JS_GLOBALSTASH_INDEX_COMPONENTS);
    duk_get_prop_string(ctx, -1, className_.CString());

    // FIXME: This is happening in editor
    if (!duk_is_function(ctx, -1))
    {
        duk_pop_n(ctx, 3);
        return;
    }

    js_push_class_object_instance(ctx, this);

    if (duk_pcall(ctx, 1) != 0)
    {
        vm_->SendJSErrorEvent();
    }
    else
    {
        scriptObject_ = this->JSGetHeapPtr();
    }

    if (scriptObject_)
    {
        GetScriptMethods();
        UpdateEventSubscription();
    }


    duk_pop_n(ctx, 2);
}

void JSComponent::HandleSceneUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!scriptObject_)
        return;

    assert(!destroyed_);

    assert(JSGetHeapPtr());

    using namespace SceneUpdate;

    float timeStep = eventData[P_TIMESTEP].GetFloat();

    duk_context* ctx = vm_->GetJSContext();

    if (!started_)
    {
        started_ = true;

        if (methods_[JSMETHOD_START])
        {
            duk_push_heapptr(ctx, methods_[JSMETHOD_START]);
            if (duk_pcall(ctx, 0) != 0)
            {
                vm_->SendJSErrorEvent();
            }

            duk_pop(ctx);
        }
    }

    if (methods_[JSMETHOD_UPDATE])
    {        
        duk_push_heapptr(ctx, methods_[JSMETHOD_UPDATE]);
        duk_push_number(ctx, timeStep);

        if ( duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS)
        {
            if (duk_is_object(ctx, -1))
            {
                vm_->SendJSErrorEvent();
            }
            else
            {
                assert(0);
            }
        }

        duk_pop(ctx);
    }
    else
    {
        Scene* scene = GetScene();
        if (scene)
            UnsubscribeFromEvent(scene, E_SCENEUPDATE);
        subscribed_ = false;
    }
}

void JSComponent::UpdateEventSubscription()
{
    Scene* scene = GetScene();
    if (!scene)
    {
        LOGWARNING("Node is detached from scene, can not subscribe script object to update events");
        return;
    }

    bool enabled = scriptObject_ && IsEnabledEffective();

    if (enabled)
    {
        // we get at least one scene update if not started
        if (!subscribed_ && (!started_ || (methods_[JSMETHOD_UPDATE] || methods_[JSMETHOD_DELAYEDSTART] )))
        {
            SubscribeToEvent(scene, E_SCENEUPDATE, HANDLER(JSComponent, HandleSceneUpdate));
            subscribed_ = true;
        }

        if (!subscribedPostFixed_)
        {
            if (methods_[JSMETHOD_POSTUPDATE])
                SubscribeToEvent(scene, E_SCENEPOSTUPDATE, HANDLER(JSComponent, HandleScenePostUpdate));

#ifdef ATOMIC_PHYSICS
            if (methods_[JSMETHOD_FIXEDUPDATE] || methods_[JSMETHOD_FIXEDPOSTUPDATE])
            {
                PhysicsWorld* world = scene->GetOrCreateComponent<PhysicsWorld>();
                if (world)
                {
                    if (methods_[JSMETHOD_FIXEDUPDATE])
                        SubscribeToEvent(world, E_PHYSICSPRESTEP, HANDLER(JSComponent, HandlePhysicsPreStep));
                    if (methods_[JSMETHOD_FIXEDPOSTUPDATE])
                        SubscribeToEvent(world, E_PHYSICSPOSTSTEP, HANDLER(JSComponent, HandlePhysicsPostStep));
                }
                else
                    LOGERROR("No physics world, can not subscribe script object to fixed update events");
            }
#endif
            subscribedPostFixed_ = true;
        }

        if (methods_[JSMETHOD_TRANSFORMCHANGED])
            node_->AddListener(this);
    }
    else
    {
        if (subscribed_)
        {
            UnsubscribeFromEvent(scene, E_SCENEUPDATE);
            subscribed_ = false;
        }

        if (subscribedPostFixed_)
        {
            UnsubscribeFromEvent(scene, E_SCENEPOSTUPDATE);
#ifdef ATOMIC_PHYSICS
            PhysicsWorld* world = scene->GetComponent<PhysicsWorld>();
            if (world)
            {
                UnsubscribeFromEvent(world, E_PHYSICSPRESTEP);
                UnsubscribeFromEvent(world, E_PHYSICSPOSTSTEP);
            }
#endif

            subscribedPostFixed_ = false;
        }

        if (methods_[JSMETHOD_TRANSFORMCHANGED])
            node_->RemoveListener(this);
    }
}


void JSComponent::HandleScenePostUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!scriptObject_)
        return;

    assert(!destroyed_);

    using namespace ScenePostUpdate;

    if (methods_[JSMETHOD_POSTUPDATE])
    {
        duk_context* ctx = vm_->GetJSContext();
        duk_push_heapptr(ctx, methods_[JSMETHOD_POSTUPDATE]);
        duk_push_number(ctx, eventData[P_TIMESTEP].GetFloat());
        duk_pcall(ctx, 1);
        duk_pop(ctx);
    }
}

#ifdef ATOMIC_PHYSICS
void JSComponent::HandlePhysicsPreStep(StringHash eventType, VariantMap& eventData)
{
    if (!scriptObject_)
        return;

    assert(!destroyed_);

    using namespace PhysicsPreStep;

    float timeStep = eventData[P_TIMESTEP].GetFloat();

    if (methods_[JSMETHOD_FIXEDUPDATE])
    {
        duk_context* ctx = vm_->GetJSContext();
        duk_push_heapptr(ctx, methods_[JSMETHOD_FIXEDUPDATE]);
        duk_push_number(ctx, timeStep);
        duk_pcall(ctx, 1);
        duk_pop(ctx);
    }
}

void JSComponent::HandlePhysicsPostStep(StringHash eventType, VariantMap& eventData)
{
    if (!scriptObject_)
        return;

    assert(!destroyed_);

    using namespace PhysicsPostStep;

    VariantVector parameters;
    parameters.Push(eventData[P_TIMESTEP]);
}
#endif
void JSComponent::HandleScriptEvent(StringHash eventType, VariantMap& eventData)
{
    if (!IsEnabledEffective() || !scriptObject_)
        return;

    assert(!destroyed_);

    if (scriptEventFunctions_.Contains(eventType))
    {

        duk_context* ctx = vm_->GetJSContext();
        JS_HEAP_PTR function = scriptEventFunctions_[eventType];

        if (eventType == E_PHYSICSBEGINCONTACT2D || E_PHYSICSENDCONTACT2D)
        {
            using namespace PhysicsBeginContact2D;
            PhysicsWorld2D* world = static_cast<PhysicsWorld2D*>(eventData[P_WORLD].GetPtr());
            RigidBody2D* bodyA = static_cast<RigidBody2D*>(eventData[P_BODYA].GetPtr());
            RigidBody2D* bodyB = static_cast<RigidBody2D*>(eventData[P_BODYB].GetPtr());
            Node* nodeA = static_cast<Node*>(eventData[P_NODEA].GetPtr());
            Node* nodeB = static_cast<Node*>(eventData[P_NODEB].GetPtr());

            duk_push_heapptr(ctx, function);
            js_push_class_object_instance(ctx, world);
            js_push_class_object_instance(ctx, bodyA);
            js_push_class_object_instance(ctx, bodyB);
            js_push_class_object_instance(ctx, nodeA);
            js_push_class_object_instance(ctx, nodeB);

            if (duk_pcall(ctx, 5) != 0)
            {
                vm_->SendJSErrorEvent();
            }

            duk_pop(ctx);

        }
#ifdef ATOMIC_PHYSICS
        else if (eventType == E_NODECOLLISION)
        {
            // Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
            using namespace NodeCollision;
            MemoryBuffer contacts(eventData[P_CONTACTS].GetBuffer());

            while (!contacts.IsEof())
            {
                Vector3 contactPosition = contacts.ReadVector3();
                Vector3 contactNormal = contacts.ReadVector3();
                float contactDistance = contacts.ReadFloat();
                float contactImpulse = contacts.ReadFloat();

                duk_push_heapptr(ctx, function);

                duk_push_array(ctx);
                duk_push_number(ctx, contactPosition.x_);
                duk_put_prop_index(ctx, -2, 0);
                duk_push_number(ctx, contactPosition.y_);
                duk_put_prop_index(ctx, -2, 1);
                duk_push_number(ctx, contactPosition.z_);
                duk_put_prop_index(ctx, -2, 2);

                duk_push_array(ctx);
                duk_push_number(ctx, contactNormal.x_);
                duk_put_prop_index(ctx, -2, 0);
                duk_push_number(ctx, contactNormal.y_);
                duk_put_prop_index(ctx, -2, 1);
                duk_push_number(ctx, contactNormal.z_);
                duk_put_prop_index(ctx, -2, 2);

                duk_call(ctx, 2);
                duk_pop(ctx);

            }

        }
#endif
        else
        {
            duk_push_heapptr(ctx, function);
            if (duk_pcall(ctx, 0) != 0)
            {
                vm_->SendJSErrorEvent();
            }

            duk_pop(ctx);

        }
    }

}


void JSComponent::GetScriptMethods()
{
    if (!scriptObject_)
        return;

    duk_context* ctx = vm_->GetJSContext();
    duk_push_heapptr(ctx, scriptObject_);

    for (unsigned i = 0; i < MAX_JSSCRIPT_METHODS; ++i)
    {
        duk_get_prop_string(ctx, -1, methodDeclarations[i]);
        if (duk_is_function(ctx, -1))
        {
            methods_[i] = duk_get_heapptr(ctx, -1);
        }

        duk_pop(ctx);
    }

    duk_pop(ctx);

}




}
