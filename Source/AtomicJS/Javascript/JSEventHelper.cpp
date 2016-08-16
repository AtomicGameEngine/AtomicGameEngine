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

#include <Atomic/UI/UIEvents.h>

#include "JSVM.h"
#include "JSEventHelper.h"

namespace Atomic
{


JSEventDispatcher::JSEventDispatcher(Context* context) :
    Object(context)
{

}

JSEventDispatcher::~JSEventDispatcher()
{
}

void JSEventDispatcher::BeginSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData)
{
}

void JSEventDispatcher::EndSendEvent(Context* context, Object* sender, StringHash eventType, VariantMap& eventData)
{
    if (!jsEvents_.Contains(eventType))
        return;

    JSVM* vm = JSVM::GetJSVM(NULL);

    if (!vm)
        return;

    duk_context* ctx = vm->GetJSContext();

    duk_push_global_stash(ctx);
    duk_get_prop_index(ctx, -1, JS_GLOBALSTASH_VARIANTMAP_CACHE);

    duk_push_pointer(ctx, (void*) &eventData);
    duk_get_prop(ctx, -2);


    // If this issue is addressed, revisit this to simply remove
    // the variantmap object from the cache, if the user explicitly
    // keeps the event object alive in a local closure, that is allowed
    // (though, will keep object properties from being GC'd)
    // https://github.com/svaarala/duktape/issues/229

    // Ok, this is unfortunate, in an event callback it is possible
    // to capture the Proxy object which represents the event VariantMap
    // in a function() {} closure, which will keep the event data alive
    // until the function happens to be gc'd (it is a member of the eventhandler)
    // which will leave things like scenes up if there was a P_SCENE event data
    // member, etc
    // So, we need to check if we have an object in the variant map cache
    // and thense call it's delete property method on the Proxy, which will clear
    // all the data (the proxy can still be alive as a captured local, though
    // the members won't be held
    // This all makes it that much more important that the pointer to eventData
    // is consistent across entire event, otherwise references may be held

    // see note above about: https://github.com/svaarala/duktape/issues/229

    if (duk_is_object(ctx, -1))
    {
        // deletes all properties, thus freeing references, even if
        // the variant map object is held onto by script (it will be invalid, post
        // event send)
        // see JSAPI.cpp variantmap_property_deleteproperty
        duk_del_prop_index(ctx, -1, 0);

        duk_push_pointer(ctx, (void*) &eventData);
        duk_push_undefined(ctx);

        // clear the variant map object from the cache
        duk_put_prop(ctx, -4);

    }

    duk_pop_3(ctx);

}

JSEventHelper::JSEventHelper(Context* context, Object* object) :
    Object(context),
    object_(object)

{

}

JSEventHelper::~JSEventHelper()
{
    Clear();
}

void JSEventHelper::Clear()
{
    UnsubscribeFromAllEvents();

    if (!object_.Expired())
    {
        object_->UnsubscribeFromEventReceiver(this);
    }
    
}

void JSEventHelper::AddEventHandler(StringHash eventType)
{
    GetSubsystem<JSEventDispatcher>()->RegisterJSEvent(eventType);

    // subscribe using object, so unsubscribing from object and not the event helper works
    object_->SubscribeToEvent(eventType, ATOMIC_HANDLER(JSEventHelper, HandleEvent));
}

void JSEventHelper::AddEventHandler(Object* sender, StringHash eventType)
{
    GetSubsystem<JSEventDispatcher>()->RegisterJSEvent(eventType);

    // subscribe using object, so unsubscribing from object and not the event helper works
    object_->SubscribeToEvent(sender, eventType, ATOMIC_HANDLER(JSEventHelper, HandleEvent));
}

void JSEventHelper::HandleEvent(StringHash eventType, VariantMap& eventData)
{
    if (object_.Expired())
        return;

    JSVM* vm = JSVM::GetJSVM(0);
    duk_context* ctx = vm->GetJSContext();

    duk_idx_t top = duk_get_top(ctx);

    js_push_class_object_instance(ctx, this);

    duk_get_prop_string(ctx, -1, "__eventHelperFunctions");

    assert(duk_is_object(ctx, -1));

    duk_get_prop_string(ctx, -1, eventType.ToString().CString());

    if (duk_is_function(ctx, -1))
    {
        // look in the variant map cache
        duk_push_global_stash(ctx);
        duk_get_prop_index(ctx, -1, JS_GLOBALSTASH_VARIANTMAP_CACHE);
        duk_push_pointer(ctx, (void*) &eventData);
        duk_get_prop(ctx, -2);

        if (!duk_is_object(ctx, -1))
        {
            // pop result
            duk_pop(ctx);

            // we need to push a new variant map and store to cache
            // the cache object will be cleared at the send end in  the
            // global listener above
            js_push_variantmap(ctx, eventData);
            duk_push_pointer(ctx, (void*) &eventData);
            duk_dup(ctx, -2);
            duk_put_prop(ctx, -4);

        }

        duk_remove(ctx, -2); // vmap cache
        duk_remove(ctx, -2); // global stash

        if (duk_pcall(ctx, 1) != 0)
        {
            vm->SendJSErrorEvent();
        }
        else
        {
            // For widget events, need to check return value
            // and set whether handled
            if (eventType == E_WIDGETEVENT)
            {
                if (duk_is_boolean(ctx, -1))
                {
                    if (duk_to_boolean(ctx, -1))
                    {
                        eventData[WidgetEvent::P_HANDLED] = true;
                    }
                }
            }
        }
    }

    duk_set_top(ctx, top);

}

}
