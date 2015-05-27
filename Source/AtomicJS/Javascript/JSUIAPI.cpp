// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <TurboBadger/tb_window.h>

#include "JSUIAPI.h"
#include "JSVM.h"

#include <Atomic/UI/UI.h>
#include <Atomic/UI/UISelectItem.h>
#include <Atomic/UI/UIMenuWindow.h>
#include <Atomic/UI/UIButton.h>
#include <Atomic/UI/UIWindow.h>

namespace Atomic
{

static int UIButton_Popup(duk_context* ctx)
{

    if (!duk_is_object(ctx, 0))
    {
        duk_push_string(ctx, "UIButton.popup first argument must be an object");
        duk_throw(ctx);
    }
    if (!duk_is_callable(ctx, 1))
    {
        duk_push_string(ctx, "UIButton.popup second argument must be callable");
        duk_throw(ctx);
    }

    JSVM* vm = JSVM::GetJSVM(ctx);

    duk_enum(ctx, 0, DUK_ENUM_OWN_PROPERTIES_ONLY);

    UISelectItemSource* source = new UISelectItemSource(vm->GetContext());

    while (duk_next(ctx, -1, 0)) {

        String key = duk_get_string(ctx, -1);

        duk_get_prop(ctx, 0);

        if (duk_is_array(ctx, -1))
        {
            // need to support this, for skin image, etc
            assert(0);
        }
        else if (duk_is_string(ctx, -1))
        {
            // id
            String id = duk_get_string(ctx, -1);            
            source->AddItem(new UISelectItem(vm->GetContext(), key, id));
        }
        else
        {
            duk_push_string(ctx, "UIButton.popup data object key is not an array or string");
            duk_throw(ctx);
        }

        duk_pop(ctx);  // pop key value
    }

    duk_pop(ctx);  // pop enum object

    duk_push_this(ctx);

    duk_dup(ctx, 1);
    duk_put_prop_string(ctx, -2, "__popup_menu_callback");

    UIButton* button = js_to_class_instance<UIButton>(ctx, -1, 0);
    UIMenuWindow* menuWindow = new UIMenuWindow(vm->GetContext(), button, "__popup-menu");
    menuWindow->Show(source);
    duk_pop(ctx);

    return 0;
}

int UIWindow_GetResizeToFitContentRect(duk_context* ctx)
{
    duk_push_this(ctx);
    UIWindow* window = js_to_class_instance<UIWindow>(ctx, -1, 0);
    duk_pop(ctx);

    tb::TBWindow* tbwindow = (tb::TBWindow*) window->GetInternalWidget();

    tb::TBRect rect = tbwindow->GetResizeToFitContentRect();

    duk_push_object(ctx);
    duk_push_number(ctx, rect.x);
    duk_put_prop_string(ctx, -2, "x");
    duk_push_number(ctx, rect.y);
    duk_put_prop_string(ctx, -2, "y");
    duk_push_number(ctx, rect.w);
    duk_put_prop_string(ctx, -2, "width");
    duk_push_number(ctx, rect.h);
    duk_put_prop_string(ctx, -2, "height");
    return 1;

}

int UI_DebugGetUIKeepAliveCount(duk_context* ctx)
{
    duk_push_global_stash(ctx);
    duk_get_prop_string(ctx, -1, "__jsui_widgetkeepalive");

    duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);

    double count = 0;

    while (duk_next(ctx, -1 , 0)) {

        duk_pop(ctx);  /* pop_key */
        count++;
    }

    duk_pop_n(ctx, 3);  /* pop enum object, keep alive object, and stash */

    duk_push_number(ctx, count);

    return 1;
}

int UI_DebugGetWrappedWidgetCount(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    UI* ui = vm->GetSubsystem<UI>();

    duk_push_number(ctx, (double) ui->DebugGetWrappedWidgetCount());
    return 1;
}

int UI_DebugShowSettingsWindow(duk_context* ctx)
{
    UIWidget* widget = js_to_class_instance<UIWidget>(ctx, 0, 0);

    if (!widget)
        return 0;

#ifdef TB_RUNTIME_DEBUG_INFO
    if (widget->GetInternalWidget())
        tb::ShowDebugInfoSettingsWindow(widget->GetInternalWidget());
#endif


    return 0;
}

int UI_Init(duk_context* ctx)
{
    JSVM* vm = JSVM::GetJSVM(ctx);
    UI* ui = vm->GetSubsystem<UI>();

    // TODO: take a config object
    ui->Initialize("DefaultUI/language/lng_en.tb.txt");
    ui->LoadSkin("DefaultUI/skin/skin.tb.txt", "Skin/skin.ui.txt");
    ui->AddFont("DefaultUI/fonts/vera.ttf", "Vera");

    ui->SetDefaultFont("Vera", 12);

    return 0;
}

void jsapi_init_ui(JSVM* vm)
{
    duk_context* ctx = vm->GetJSContext();

    // UI object
    duk_get_global_string(ctx, "Atomic");

    duk_push_object(ctx);

    duk_push_c_function(ctx, UI_Init, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "__init");

    duk_push_c_function(ctx, UI_DebugGetWrappedWidgetCount, 0);
    duk_put_prop_string(ctx, -2, "debugGetWrappedWidgetCount");

    duk_push_c_function(ctx, UI_DebugGetUIKeepAliveCount, 0);
    duk_put_prop_string(ctx, -2, "debugGetUIKeepAliveCount");

    duk_push_c_function(ctx, UI_DebugShowSettingsWindow, 1);
    duk_put_prop_string(ctx, -2, "debugShowSettingsWindow");


    // Layout

    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_NONE);
    duk_put_prop_string(ctx, -2, "GRAVITY_NONE");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_LEFT);
    duk_put_prop_string(ctx, -2, "GRAVITY_LEFT");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_RIGHT);
    duk_put_prop_string(ctx, -2, "GRAVITY_RIGHT");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_TOP);
    duk_put_prop_string(ctx, -2, "GRAVITY_TOP");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_BOTTOM);
    duk_put_prop_string(ctx, -2, "GRAVITY_BOTTOM");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_LEFT_RIGHT);
    duk_put_prop_string(ctx, -2, "GRAVITY_LEFT_RIGHT");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_TOP_BOTTOM);
    duk_put_prop_string(ctx, -2, "GRAVITY_TOP_BOTTOM");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_ALL);
    duk_put_prop_string(ctx, -2, "GRAVITY_ALL");
    duk_push_number(ctx, (double) tb::WIDGET_GRAVITY_DEFAULT);
    duk_put_prop_string(ctx, -2, "GRAVITY_DEFAULT");

    duk_push_number(ctx, (double) tb::AXIS_X);
    duk_put_prop_string(ctx, -2, "AXIS_X");
    duk_push_number(ctx, (double) tb::AXIS_Y);
    duk_put_prop_string(ctx, -2, "AXIS_Y");

    duk_push_number(ctx, (double) tb::LAYOUT_SIZE_GRAVITY);
    duk_put_prop_string(ctx, -2, "LAYOUT_SIZE_GRAVITY");
    duk_push_number(ctx, (double) tb::LAYOUT_SIZE_PREFERRED);
    duk_put_prop_string(ctx, -2, "LAYOUT_SIZE_PREFERRED");
    duk_push_number(ctx, (double) tb::LAYOUT_SIZE_AVAILABLE);
    duk_put_prop_string(ctx, -2, "LAYOUT_SIZE_AVAILABLE");

    duk_push_number(ctx, (double) tb::LAYOUT_DISTRIBUTION_PREFERRED);
    duk_put_prop_string(ctx, -2, "LAYOUT_DISTRIBUTION_PREFERRED");
    duk_push_number(ctx, (double) tb::LAYOUT_DISTRIBUTION_AVAILABLE);
    duk_put_prop_string(ctx, -2, "LAYOUT_DISTRIBUTION_AVAILABLE");
    duk_push_number(ctx, (double) tb::LAYOUT_DISTRIBUTION_GRAVITY);
    duk_put_prop_string(ctx, -2, "LAYOUT_DISTRIBUTION_GRAVITY");

    duk_push_number(ctx, (double) tb::LAYOUT_POSITION_CENTER);
    duk_put_prop_string(ctx, -2, "LAYOUT_POSITION_CENTER");
    duk_push_number(ctx, (double) tb::LAYOUT_POSITION_LEFT_TOP);
    duk_put_prop_string(ctx, -2, "LAYOUT_POSITION_LEFT_TOP");
    duk_push_number(ctx, (double) tb::LAYOUT_POSITION_RIGHT_BOTTOM);
    duk_put_prop_string(ctx, -2, "LAYOUT_POSITION_RIGHT_BOTTOM");
    duk_push_number(ctx, (double) tb::LAYOUT_POSITION_GRAVITY);
    duk_put_prop_string(ctx, -2, "LAYOUT_POSITION_GRAVITY");

    duk_push_number(ctx, (double) tb::LAYOUT_DISTRIBUTION_POSITION_CENTER);
    duk_put_prop_string(ctx, -2, "LAYOUT_DISTRIBUTION_POSITION_CENTER");
    duk_push_number(ctx, (double) tb::LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP);
    duk_put_prop_string(ctx, -2, "LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP");
    duk_push_number(ctx, (double) tb::LAYOUT_DISTRIBUTION_POSITION_RIGHT_BOTTOM);
    duk_put_prop_string(ctx, -2, "LAYOUT_DISTRIBUTION_POSITION_RIGHT_BOTTOM");

    // Window Settings
    duk_push_number(ctx, (double) tb::WINDOW_SETTINGS_NONE);
    duk_put_prop_string(ctx, -2, "WINDOW_SETTINGS_NONE");
    duk_push_number(ctx, (double) tb::WINDOW_SETTINGS_TITLEBAR);
    duk_put_prop_string(ctx, -2, "WINDOW_SETTINGS_TITLEBAR");
    duk_push_number(ctx, (double) tb::WINDOW_SETTINGS_RESIZABLE);
    duk_put_prop_string(ctx, -2, "WINDOW_SETTINGS_RESIZABLE");
    duk_push_number(ctx, (double) tb::WINDOW_SETTINGS_CLOSE_BUTTON);
    duk_put_prop_string(ctx, -2, "WINDOW_SETTINGS_CLOSE_BUTTON");
    duk_push_number(ctx, (double) tb::WINDOW_SETTINGS_CAN_ACTIVATE);
    duk_put_prop_string(ctx, -2, "WINDOW_SETTINGS_CAN_ACTIVATE");
    duk_push_number(ctx, (double) tb::WINDOW_SETTINGS_DEFAULT);
    duk_put_prop_string(ctx, -2, "WINDOW_SETTINGS_DEFAULT");

    duk_push_number(ctx, (double) tb::WIDGET_STATE_NONE);
    duk_put_prop_string(ctx, -2, "WIDGET_STATE_NONE");
    duk_push_number(ctx, (double) tb::WIDGET_STATE_DISABLED);
    duk_put_prop_string(ctx, -2, "WIDGET_STATE_DISABLED");
    duk_push_number(ctx, (double) tb::WIDGET_STATE_FOCUSED);
    duk_put_prop_string(ctx, -2, "WIDGET_STATE_FOCUSED");
    duk_push_number(ctx, (double) tb::WIDGET_STATE_PRESSED);
    duk_put_prop_string(ctx, -2, "WIDGET_STATE_PRESSED");
    duk_push_number(ctx, (double) tb::WIDGET_STATE_SELECTED);
    duk_put_prop_string(ctx, -2, "WIDGET_STATE_SELECTED");
    duk_push_number(ctx, (double) tb::WIDGET_STATE_HOVERED);
    duk_put_prop_string(ctx, -2, "WIDGET_STATE_HOVERED");
    duk_push_number(ctx, (double) tb::WIDGET_STATE_ALL);
    duk_put_prop_string(ctx, -2, "WIDGET_STATE_ALL");

    // Events
    duk_push_number(ctx, (double) tb::EVENT_TYPE_CLICK);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_CLICK");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_LONG_CLICK);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_LONG_CLICK");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_POINTER_DOWN);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_POINTER_DOWN");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_POINTER_UP);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_POINTER_UP");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_POINTER_MOVE);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_POINTER_MOVE");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_RIGHT_POINTER_DOWN);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_RIGHT_POINTER_DOWN");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_RIGHT_POINTER_UP);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_RIGHT_POINTER_UP");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_WHEEL);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_WHEEL");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_CHANGED);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_CHANGED");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_KEY_DOWN);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_KEY_DOWN");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_KEY_UP);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_KEY_UP");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_SHORTCUT);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_SHORTCUT");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_CONTEXT_MENU);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_CONTEXT_MENU");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_FILE_DROP);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_FILE_DROP");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_TAB_CHANGED);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_TAB_CHANGED");
    duk_push_number(ctx, (double) tb::EVENT_TYPE_CUSTOM);
    duk_put_prop_string(ctx, -2, "EVENT_TYPE_CUSTOM");

    duk_put_prop_string(ctx, -2, "UI");

    duk_pop(ctx);

    js_class_get_prototype(ctx, "Atomic", "UIButton");
    duk_push_c_function(ctx, UIButton_Popup, 2);
    duk_put_prop_string(ctx, -2, "popup");
    duk_pop(ctx);

    js_class_get_prototype(ctx, "Atomic", "UIWindow");
    duk_push_c_function(ctx, UIWindow_GetResizeToFitContentRect, 0);
    duk_put_prop_string(ctx, -2, "getResizeToFitContentRect");
    duk_pop(ctx);


}

}
