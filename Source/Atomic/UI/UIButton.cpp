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

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "UIEvents.h"
#include "UI.h"
#include "UIButton.h"

using namespace tb;

namespace Atomic
{

UIButton::UIButton(Context* context, bool createWidget) : UIWidget(context, false),
    emulationButton_(-1),
    urlEnabled_(true)
{
    if (createWidget)
    {
        widget_ = new TBButton();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIButton::~UIButton()
{

}

void UIButton::SetSqueezable(bool value)
{
    if (!widget_)
        return;

    ((TBButton*)widget_)->SetSqueezable(value);
}

void UIButton::SetEmulationButton(int emulationButton)
{
    emulationButton_ = emulationButton;
}

bool UIButton::OnEvent(const tb::TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (urlEnabled_)
        {
            String text = GetText();
            if (text.StartsWith("http://") || text.StartsWith("https://"))
            {
                FileSystem* fileSystem = GetSubsystem<FileSystem>();
                fileSystem->SystemOpen(text);
            }
        }
    }
    if (ev.type == EVENT_TYPE_POINTER_DOWN && emulationButton_ >= 0)
    {
        GetSubsystem<Input>()->SimulateButtonDown(emulationButton_);
    }
    if (ev.type == EVENT_TYPE_POINTER_UP && emulationButton_ >= 0)
    {
        GetSubsystem<Input>()->SimulateButtonUp(emulationButton_);
    }
    return UIWidget::OnEvent(ev);
}

}
