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
#include <TurboBadger/tb_layout.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIWidget.h"
#include "UILayout.h"

using namespace tb;

namespace Atomic
{

UILayoutParams::UILayoutParams(Context* context) : Object(context)
{
}

UILayoutParams::~UILayoutParams()
{
}


UILayout::UILayout(Context* context, UI_AXIS axis, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBLayout();
        widget_->SetDelegate(this);
        widget_->SetGravity(WIDGET_GRAVITY_ALL);
        widget_->SetAxis((AXIS) axis);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UILayout::~UILayout()
{
}

void UILayout::SetSpacing(int spacing)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetSpacing(spacing);
}

void UILayout::SetLayoutPosition(UI_LAYOUT_POSITION position)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetLayoutPosition( (LAYOUT_POSITION) position);
}

void UILayout::SetLayoutDistributionPosition(UI_LAYOUT_DISTRIBUTION_POSITION distribution_pos)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetLayoutDistributionPosition( (LAYOUT_DISTRIBUTION_POSITION) distribution_pos);

}

void UILayout::SetLayoutSize(UI_LAYOUT_SIZE size)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetLayoutSize((LAYOUT_SIZE) size);

}

void UILayout::SetAxis(UI_AXIS axis)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetAxis((AXIS) axis);
}

void UILayout::SetLayoutDistribution(UI_LAYOUT_DISTRIBUTION distribution)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetLayoutDistribution((LAYOUT_DISTRIBUTION) distribution);
}


bool UILayout::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
