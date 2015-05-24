// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "JSCore.h"
#include "JSEventHelper.h"
#include "JSVM.h"

namespace Atomic
{

static int EventHandler_Property_Get(duk_context* ctx)
{
    // targ, key, recv

    if (duk_is_string(ctx, 1))
    {
        const char* cstr = duk_to_string(ctx, 1);
        StringHash key = cstr;

        JSEventHelper* helper = js_to_class_instance<JSEventHelper>(ctx, 0, 0);
        VariantMap& data = helper->GetCurrentData();

        if (!data.Contains(key))
        {
            duk_push_undefined(ctx);
            return 1;
        }

        js_push_variant(ctx, data[key]);
        return 1;
    }

    duk_push_undefined(ctx);
    return 1;

}

static int Object_SubscribeToEvent(duk_context* ctx)
{

    int top = duk_get_top(ctx);

    Object* sender = NULL;

    StringHash eventType = StringHash::ZERO;


    if ( top == 2 ) // General notification: subscribeToEvent("ScreenMode", function() {});
    {
        if (duk_is_string(ctx, 0) && duk_is_function(ctx, 1))
        {
            eventType = duk_to_string(ctx, 0);
        }
    }
    else if (top == 3) // Listen to specific object sender subscribeToEvent(graphics, "ScreenMode", function() {});
    {
        if (duk_is_object(ctx, 0) && duk_is_string(ctx, 1) && duk_is_function(ctx, 2))
        {
            sender = js_to_class_instance<Object>(ctx, 0, 0);
            eventType = duk_to_string(ctx, 1);
        }
    }

    if ( eventType == StringHash::ZERO)
    {
        duk_push_string(ctx, "Object.subscribeToEvent() - Bad Arguments");
        duk_throw(ctx);
    }

    duk_push_this(ctx); // stack 2

    // event receiver
    Object* object = js_to_class_instance<Object>(ctx, -1, 0);

    duk_get_prop_string(ctx, -1, "__eventHelper");

    // need to setup the handler
    if (duk_is_null_or_undefined(ctx, -1))
    {
        duk_pop(ctx); // pop null or undefined

        // construct a new event helper
        js_push_class_object_instance(ctx, new JSEventHelper(object->GetContext()));

        // proxy support
        duk_get_global_string(ctx, "Proxy");

        duk_dup(ctx, -2);

        // setup property handler
        duk_push_object(ctx);
        duk_push_c_function(ctx, EventHandler_Property_Get, 3);
        duk_put_prop_string(ctx, -2, "get");
        duk_new(ctx, 2);
        duk_put_prop_string(ctx, -2, "__eventHelperProxy");

        duk_push_object(ctx);
        duk_put_prop_string(ctx, -2, "__eventHelperFunctions");

        // dup so when we set the helper is left on stack
        duk_dup_top(ctx);

        duk_put_prop_string(ctx, 2, "__eventHelper");
    }

    JSEventHelper* helper = js_to_class_instance<JSEventHelper>(ctx, -1, 0);

    duk_get_prop_string(ctx, -1, "__eventHelperFunctions");
    assert(duk_is_object(ctx, -1));
    assert(duk_is_function(ctx, sender ? 2 : 1));
    duk_dup(ctx, sender ? 2 : 1);
    duk_put_prop_string(ctx, -2,  eventType.ToString().CString());
    duk_pop(ctx);

    if (sender)
        helper->AddEventHandler(sender, eventType);
    else
        helper->AddEventHandler(eventType);

    return 0;
}

void jsapi_init_core(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    js_class_get_prototype(ctx, "AObject");
    duk_push_c_function(ctx, Object_SubscribeToEvent, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "subscribeToEvent");
    duk_pop(ctx);
}

}
