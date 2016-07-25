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
#include <TurboBadger/tb_atomic_widgets.h>

#include <Atomic/IO/Log.h>
#include <Atomic/Graphics/Texture.h>

#include "UIEvents.h"
#include "UI.h"
#include "UIColorWheel.h"

using namespace tb;

namespace Atomic
{

UIColorWheel::UIColorWheel(Context* context, bool createWidget) : UIWidget(context, false)
{
    UI* ui = GetSubsystem<UI>();

    if (createWidget)
    {
        widget_ = new TBColorWheel();
        widget_->SetDelegate(this);
        ui->WrapWidget(this, widget_);
    }

}

UIColorWheel::~UIColorWheel()
{
}

bool UIColorWheel::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

float UIColorWheel::GetHue()
{
    if (!widget_)
        return 0.0;
    return ((TBColorWheel*) widget_)->GetHue();
}

float UIColorWheel::GetSaturation()
{
    if (!widget_)
        return 0.0;
    return ((TBColorWheel*) widget_)->GetSaturation();
}

void UIColorWheel::SetHueSaturation ( float hue, float saturation )
{
     if (!widget_)
        return;
    ((TBColorWheel*) widget_)->SetHueSaturation ( hue, saturation );
}

}
