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

#include <Atomic/Core/ProcessUtils.h>

#include "JSCore.h"
#include "JSEventHelper.h"
#include "JSVM.h"

namespace Atomic
{

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

    duk_push_this(ctx);

    // event receiver
    Object* object = js_to_class_instance<Object>(ctx, -1, 0);

    duk_get_prop_string(ctx, -1, "__eventHelper");

    // need to setup the handler
    if (duk_is_null_or_undefined(ctx, -1))
    {
        duk_pop(ctx); // pop null or undefined

        // construct a new event helper
        JSEventHelper* eventHelper = new JSEventHelper(object->GetContext(), object);
        eventHelper->SetInstantiationType(INSTANTIATION_JAVASCRIPT);
        js_push_class_object_instance(ctx, eventHelper);

        duk_push_object(ctx);
        duk_put_prop_string(ctx, -2, "__eventHelperFunctions");

        // dup so when we set the helper is left on stack
        duk_dup_top(ctx);

        duk_put_prop_string(ctx, -3, "__eventHelper");
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

static int Object_SendEvent(duk_context* ctx)
{
    int top = duk_get_top(ctx);

    duk_push_this(ctx);

    // event sender
    Object* sender = js_to_class_instance<Object>(ctx, -1, 0);

    if (top == 1)
    {
        sender->SendEvent(duk_to_string(ctx, 0));
    }
    else if (top == 2)
    {
        if (duk_is_object(ctx, 1)) {

            VariantMap sendEventVMap;
            js_object_to_variantmap(ctx, 1, sendEventVMap);

            sender->SendEvent(duk_to_string(ctx, 0), sendEventVMap);

        }

    }

    return 0;

}

static int Atomic_GetArguments(duk_context* ctx)
{
    duk_push_array(ctx);

    for (unsigned i = 0; i < GetArguments().Size(); i++)
    {
        duk_push_string(ctx, GetArguments()[i].CString());
        duk_put_prop_index(ctx, -2, i);
    }

    return 1;

}

void jsapi_init_core(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    duk_get_global_string(ctx, "Atomic");

    duk_push_c_function(ctx, Atomic_GetArguments, 0);
    duk_put_prop_string(ctx, -2, "getArguments");

    duk_pop(ctx); // pop Atomic object

    js_class_get_prototype(ctx, "Atomic", "AObject");
    duk_push_c_function(ctx, Object_SubscribeToEvent, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "subscribeToEvent");
    duk_push_c_function(ctx, Object_SendEvent, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "sendEvent");
    duk_pop(ctx); // pop AObject prototype
}

}
