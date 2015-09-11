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

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_window.h>

#include "../IO/Log.h"

#include "UI.h"
#include "UIEvents.h"
#include "UIWindow.h"

using namespace tb;

namespace Atomic
{

UIWindow::UIWindow(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBWindow();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}
UI_WINDOW_SETTINGS UIWindow::GetSettings()
{
    if (!widget_)
        return UI_WINDOW_SETTINGS_DEFAULT;

    return (UI_WINDOW_SETTINGS) ((TBWindow*)widget_)->GetSettings();

}

void UIWindow::SetSettings(UI_WINDOW_SETTINGS settings)
{
    if (!widget_)
        return;

    ((TBWindow*)widget_)->SetSettings((WINDOW_SETTINGS) settings);
}

void UIWindow::ResizeToFitContent()
{
    if (!widget_)
        return;

    ((TBWindow*)widget_)->ResizeToFitContent();

}

void UIWindow::Close()
{
    if (!widget_)
        return;

    ((TBWindow*)widget_)->Close();

}


void UIWindow::AddChild(UIWidget *child)
{
    if (!widget_ || !child->GetInternalWidget())
        return;

    UIWidget::AddChild(child);

    // this is to get proper padding, this may cause problems
    // as this is called from the widget_reader, and not as part of
    // AddChild.  This may also need to be called from other widgets
    // that have padding, etc
    widget_->OnInflateChild(child->GetInternalWidget());

}

UIWindow::~UIWindow()
{
}

bool UIWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
