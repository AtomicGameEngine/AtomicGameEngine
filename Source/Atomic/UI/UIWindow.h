
#pragma once

#include<TurboBadger/tb_window.h>

#include "UIWidget.h"

namespace Atomic
{


enum UI_WINDOW_SETTINGS {
    ///< Chrome less window without any other settings.
    UI_WINDOW_SETTINGS_NONE			= tb::WINDOW_SETTINGS_NONE,
    ///< Show a title bar that can also move the window.
    UI_WINDOW_SETTINGS_TITLEBAR		= tb::WINDOW_SETTINGS_TITLEBAR,
    ///< Show a widget for resizing the window.
    UI_WINDOW_SETTINGS_RESIZABLE	= tb::WINDOW_SETTINGS_RESIZABLE,
    ///< Show a widget for closing the window.
    UI_WINDOW_SETTINGS_CLOSE_BUTTON	= tb::WINDOW_SETTINGS_CLOSE_BUTTON,
    ///< Can be activated and deactivate other windows.
    UI_WINDOW_SETTINGS_CAN_ACTIVATE	= tb::WINDOW_SETTINGS_CAN_ACTIVATE,

    UI_WINDOW_SETTINGS_DEFAULT = tb::WINDOW_SETTINGS_DEFAULT
};


class UIWindow : public UIWidget
{
    OBJECT(UIWindow)

    public:

        UIWindow(Context* context, bool createWidget = true);
    virtual ~UIWindow();

    UI_WINDOW_SETTINGS GetSettings();
    void SetSettings(UI_WINDOW_SETTINGS settings);

    void ResizeToFitContent();

    void AddChild(UIWidget *child);

    void Close();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
