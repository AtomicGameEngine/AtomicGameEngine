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
#include <TurboBadger/tb_toggle_container.h>

#include <Atomic/IO/Log.h>

#include "UIEvents.h"
#include "UI.h"
#include "UISection.h"

using namespace tb;

namespace Atomic
{

UISection::UISection(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBSection();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UISection::~UISection()
{

}

bool UISection::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

void UISection::AddChild(UIWidget* child)
{
    if (!widget_ || !child || !child->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChild(child->GetInternalWidget());
}

void UISection::AddChildAfter(UIWidget* child, UIWidget* otherChild)
{
    if (!widget_ || !child || !child->GetInternalWidget() || !otherChild || !otherChild->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChildRelative(child->GetInternalWidget(), tb::WIDGET_Z_REL_AFTER, otherChild->GetInternalWidget());
}

void UISection::AddChildBefore(UIWidget* child, UIWidget* otherChild)
{
    if (!widget_ || !child || !child->GetInternalWidget() || !otherChild || !otherChild->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChildRelative(child->GetInternalWidget(), tb::WIDGET_Z_REL_BEFORE, otherChild->GetInternalWidget());
}

void UISection::AddChildRelative(UIWidget* child, UI_WIDGET_Z_REL z, UIWidget* reference)
{
    if (!widget_ || !child || !child->GetInternalWidget() || !reference || !reference->GetInternalWidget())
        return;

    TBSafeCast<TBSection>(widget_)->AddToggleChildRelative(child->GetInternalWidget(), (WIDGET_Z_REL)z, reference->GetInternalWidget());
}

void UISection::RemoveChild(UIWidget* child, bool cleanup)
{
    if (!widget_ || !child)
        return;

    TBWidget* childw = child->GetInternalWidget();

    if (!childw)
        return;

    TBSafeCast<TBSection>(widget_)->RemoveToggleChild(childw);
}

void UISection::DeleteAllChildren()
{
    if (!widget_)
        return;

    TBSafeCast<TBSection>(widget_)->DeleteAllChildren();
}

UIWidget* UISection::GetFirstChild()
{
    if (!widget_)
        return NULL;

    return GetSubsystem<UI>()->WrapWidget(TBSafeCast<TBSection>(widget_)->GetFirstToggleChild());
}

UIWidget* UISection::GetWidget(const String& id)
{
    if (!widget_)
        return 0;

    TBWidget* child = TBSafeCast<TBSection>(widget_)->GetToggleWidgetById(TBID(id.CString()));

    if (!child)
        return 0;

    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}
}
