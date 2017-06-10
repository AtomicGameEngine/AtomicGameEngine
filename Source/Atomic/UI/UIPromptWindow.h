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


#include <TurboBadger/tb_atomic_widgets.h>

#include "UIWindow.h"

namespace Atomic
{

class UIPromptWindow : public UIWindow
{
    ATOMIC_OBJECT(UIPromptWindow, UIWindow)

public:

    UIPromptWindow(Context* context, UIWidget* target, const String& id, bool createWidget = true);
    virtual ~UIPromptWindow();

    void Show(const String& title, const String& message, const String& preset, int dimmer=0, int width=0, int height=0);

protected:

    UIWidget* GetEditWidget();

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};


}
