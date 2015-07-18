
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
    duk_push_undefined(ctx);
    duk_put_prop(ctx, -3);

    duk_pop_2(ctx);

}

JSEventHelper::JSEventHelper(Context* context, Object* object) :
    Object(context),
    object_(object)

{

}

JSEventHelper::~JSEventHelper()
{

}

void JSEventHelper::AddEventHandler(StringHash eventType)
{
    GetSubsystem<JSEventDispatcher>()->RegisterJSEvent(eventType);

    // subscribe using object, so unsubscribing from object and not the event helper works
    object_->SubscribeToEvent(eventType, HANDLER(JSEventHelper, HandleEvent));
}

void JSEventHelper::AddEventHandler(Object* sender, StringHash eventType)
{
    GetSubsystem<JSEventDispatcher>()->RegisterJSEvent(eventType);

    // subscribe using object, so unsubscribing from object and not the event helper works
    object_->SubscribeToEvent(sender, eventType, HANDLER(JSEventHelper, HandleEvent));
}

void JSEventHelper::HandleEvent(StringHash eventType, VariantMap& eventData)
{
    if (object_.Null())
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
