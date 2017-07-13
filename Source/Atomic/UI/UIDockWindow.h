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

    UIDockWindow(Context* context, bool createWidget = true, const String& title = String::EMPTY, UIWidget *contentptr = NULL, int minwidth = 800, int minheight=400 );
    virtual ~UIDockWindow();

    void SetDockOrigin( String dockid );  // where using the dock button returns the content to
    UIWidget *GetDockContent();  // the content that is being swapped around
    bool HasDockContent();  // is there content in the dockwindow
    void Dock( UIWidget *target );  // move the dock window content to somewhere else
    void Show( UIWidget *host, int xpos = 50, int ypos = 50 ); // show the dock window

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
