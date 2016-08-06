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
    ATOMIC_OBJECT(UIWindow, UIWidget)

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
