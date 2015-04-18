#include <TurboBadger/tb_widgets.h>

#include <Atomic/UI/UIEvents.h>
#include <Atomic/UI/UIWidget.h>
#include "JSVM.h"
#include "JSUI.h"

namespace Atomic
{

JSUI::JSUI(Context* context) : Object(context)
{
    ctx_ = JSVM::GetJSVM(nullptr)->GetJSContext();
    SubscribeToEvent(E_WIDGETEVENT, HANDLER(JSUI, HandleWidgetEvent));
}

JSUI::~JSUI()
{

}

void JSUI::HandleWidgetEvent(StringHash eventType, VariantMap& eventData)
{
    using namespace WidgetEvent;

    UIWidget* widget = static_cast<UIWidget*>(eventData[P_TARGET].GetPtr());
    if (!widget)
        return;

    void* heapptr = widget->JSGetHeapPtr();

    if (!heapptr)
        return;

    tb::EVENT_TYPE type = (tb::EVENT_TYPE) eventData[P_TYPE].GetUInt();

    if (type == tb::EVENT_TYPE_CLICK)
    {
        int top = duk_get_top(ctx_);
        duk_push_heapptr(ctx_, heapptr);
        duk_get_prop_string(ctx_, -1, "onClick");
        if (duk_is_callable(ctx_, -1)) {
            duk_call(ctx_, 0);
        }
        duk_pop_n(ctx_, 2);
        assert(top == duk_get_top(ctx_));
        return;
    }

    eventData[P_TYPE];
    eventData[P_X];
    eventData[P_Y];
    eventData[P_DELTAX];
    eventData[P_DELTAY];
    eventData[P_COUNT];
    eventData[P_KEY];
    eventData[P_SPECIALKEY];
    eventData[P_MODIFIERKEYS];
    eventData[P_ID];
    eventData[P_TOUCH];


}

}
