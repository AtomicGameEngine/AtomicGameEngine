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

#include <TurboBadger/tb_select_item.h>
#include <TurboBadger/tb_menu_window.h>
#include "UIWidget.h"

namespace Atomic
{

class UISelectItemSource;

class UIMenuWindow : public UIWidget
{
    ATOMIC_OBJECT(UIMenuWindow, UIWidget)

public:

    UIMenuWindow(Context* context, UIWidget* target, const String& id = String::EMPTY);
    virtual ~UIMenuWindow();

    void Show(UISelectItemSource* source, int x = -1, int y = -1);

    void Close();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

    tb::TBSelectItemSource* source_;

};

}
