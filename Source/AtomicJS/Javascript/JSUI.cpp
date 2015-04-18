#include <TurboBadger/tb_widgets.h>

#include <Atomic/UI/UIEvents.h>
#include <Atomic/UI/UIWidget.h>
#include <Atomic/UI/UI.h>
#include "JSVM.h"
#include "JSUI.h"

using namespace tb;

namespace Atomic
{

JSUI::JSUI(Context* context) : Object(context)
{
    ctx_ = JSVM::GetJSVM(nullptr)->GetJSContext();
    SubscribeToEvent(E_WIDGETEVENT, HANDLER(JSUI, HandleWidgetEvent));
    SubscribeToEvent(E_WIDGETLOADED, HANDLER(JSUI, HandleWidgetLoaded));
}

JSUI::~JSUI()
{

}

void JSUI::GatherWidgets(tb::TBWidget* widget, PODVector<tb::TBWidget*>& widgets)
{
    if (widget->GetID() != TBID())
        widgets.Push(widget);

    for (TBWidget *n = widget->GetFirstChild(); n; n = n->GetNext())
    {
        GatherWidgets(n, widgets);
    }

}

void JSUI::HandleWidgetLoaded(StringHash eventType, VariantMap& eventData)
{
    using namespace WidgetLoaded;

    UIWidget* widget = static_cast<UIWidget*>(eventData[P_WIDGET].GetPtr());
    if (!widget)
        return;

    void* heapptr = widget->JSGetHeapPtr();

    if (!heapptr)
        return;

    // a loaded widget recursively gathers children which have id's and
    // stashes them in an internal array, so that they don't go out of scope
    // for instance var button = window.getWidgetByID("reveal");
    // if we didn't stash, any callback on button wouldn't work if button went out
    // of scope, which isn't expected behavior (in JS you would think the button is
    // in some way attached to the window object)

    TBWidget* tbwidget = widget->GetInternalWidget();
    assert(tbwidget);

    PODVector<TBWidget*> widgets;
    GatherWidgets(tbwidget, widgets);

    UI* ui = GetSubsystem<UI>();

    duk_push_heapptr(ctx_, heapptr);
    duk_push_array(ctx_);

    unsigned arrayCount = 0;
    for (unsigned i = 0; i < widgets.Size(); i++)
    {
        UIWidget* o =  ui->WrapWidget(widgets.At(i));

        if (!o)
            continue;

        js_push_class_object_instance(ctx_, o);

        duk_put_prop_index(ctx_, -2, arrayCount++);
    }

    duk_put_prop_string(ctx_, -2, "__child_widgets");
    duk_pop(ctx_);

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
