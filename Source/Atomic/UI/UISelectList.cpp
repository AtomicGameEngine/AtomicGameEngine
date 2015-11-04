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

#include <TurboBadger/tb_select.h>

#include "UI.h"
#include "UIEvents.h"
#include "UISelectList.h"

using namespace tb;

namespace Atomic
{

UISelectList::UISelectList(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBSelectList();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UISelectList::~UISelectList()
{

}

tb::TBSelectList* UISelectList::GetTBSelectList()
{
    return (TBSelectList*) widget_;
}

void UISelectList::SetFilter(const String& filter)
{
    if (!widget_)
        return;

    ((TBSelectList*)widget_)->SetFilter(filter.CString());
}

void UISelectList::SetValue(int value)
{
    if (!widget_)
        return;

    ((TBSelectList*)widget_)->SetValue(value);

}

double UISelectList::GetValue()
{
    if (!widget_)
        return 0;

    return (double) ((TBSelectList*)widget_)->GetValue();

}

void UISelectList::InvalidateList()
{
    if (!widget_)
        return;

    return ((TBSelectList*)widget_)->InvalidateList();
}

String UISelectList::GetSelectedItemID()
{
    if (!widget_)
        return "";

    String id_;

    TBID id = ((TBSelectList*)widget_)->GetSelectedItemID();

    GetSubsystem<UI>()->GetTBIDString(id, id_);

    return id_;

}

String UISelectList::GetHoverItemID()
{
    if (!widget_)
        return "";

    if (!TBWidget::hovered_widget)
        return "";

    TBSelectList* select = (TBSelectList*) widget_;

    if (!select->IsAncestorOf(TBWidget::hovered_widget))
    {
        return "";
    }

    String id_;
    GetSubsystem<UI>()->GetTBIDString(TBWidget::hovered_widget->GetID(), id_);

    return id_;

}

void UISelectList::SetSource(UISelectItemSource* source)
{
    if (!widget_)
        return;

    ((TBSelectList*)widget_)->SetSource(source ? source->GetTBItemSource() : NULL);
}

bool UISelectList::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
