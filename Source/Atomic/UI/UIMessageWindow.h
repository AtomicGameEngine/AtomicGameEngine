
#pragma once

#include <TurboBadger/tb_message_window.h>

#include "UIWindow.h"

namespace Atomic
{

enum UI_MESSAGEWINDOW_SETTINGS
{
    UI_MESSAGEWINDOW_SETTINGS_OK = tb::TB_MSG_OK,
    UI_MESSAGEWINDOW_SETTINGS_OK_CANCEL = tb::TB_MSG_OK_CANCEL,
    UI_MESSAGEWINDOW_SETTINGS_YES_NO = tb::TB_MSG_YES_NO
};


class UIMessageWindow : public UIWindow
{
    OBJECT(UIMessageWindow)

public:

    UIMessageWindow(Context* context, UIWidget* target, const String& id, bool createWidget = true);
    virtual ~UIMessageWindow();

    void Show(const String& title, const String& message, UI_MESSAGEWINDOW_SETTINGS settings, bool dimmer, int width, int height);

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
