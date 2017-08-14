//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#include<TurboBadger/tb_atomic_widgets.h>

#include "UIWidget.h"
#include "UIWindow.h"

namespace Atomic
{

class ATOMIC_API UIDockWindow : public UIWindow
{
    ATOMIC_OBJECT(UIDockWindow, UIWindow)

    public:
    /// UIDockWindow is the host for UI content which has been transferred from the main window.
    /// Context is a required argument, this is the application context.
    /// createWidget is if the UIDockWindow should be created, should be set to true.
    /// title is the string in the titlebar of the UIDockWindow
    /// contentptr is the pointer to the widget which the UIDockWindow will display
    /// minwidth is the minimum width for the UIDockWindow
    /// minheight is the minimum height for the UIDockWindow
    UIDockWindow(Context* context, bool createWidget = true, const String& title = String::EMPTY, UIWidget *contentptr = NULL, int minwidth = 800, int minheight=400 );
    virtual ~UIDockWindow();

    void SetDockOrigin( String dockid );  /// ID of the redock widget. If specified, pressing the dock button, will move the content there.
    UIWidget *GetDockContent();  /// This returns a pointer to the docked content.
    bool HasDockContent();  /// Returns if the UIDockWindow contains docked content.
    void Dock( UIWidget *target );  /// Transfers the dock content to the target widget
    void Show( UIWidget *host, int xpos = 50, int ypos = 50 ); /// Show the UIDockWindow, and optional position
    void SetAxis(UI_AXIS axis);   /// Axis orientation of titlebar, UI_AXIS_Y = top(default), UI_AXIS_X = left side

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
