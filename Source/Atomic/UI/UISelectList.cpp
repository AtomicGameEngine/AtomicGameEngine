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

#include "../Atomic/IO/Log.h"
#include "../Atomic/Input/Input.h"

#include "UI.h"
#include "UIEvents.h"
#include "UIDragDrop.h"
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

    SubscribeToEvent(E_UIUPDATE, ATOMIC_HANDLER(UISelectList, HandleUIUpdate));
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

int UISelectList::GetNumItems() const
{
    if (!widget_)
        return 0;

    return ((TBSelectList*)widget_)->GetNumItems();

}

void UISelectList::SelectItem(int index, bool selected)
{
    if (!widget_)
        return;

    ((TBSelectList*)widget_)->SelectItem(index, selected);

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

bool UISelectList::GetItemSelected(int index)
{
    if (!widget_)
        return false;

    return ((TBSelectList*)widget_)->GetItemSelected(index);
}

String UISelectList::GetItemID(int index)
{
    if (!widget_)
        return "";

    String _id;

    TBID id = ((TBSelectList*)widget_)->GetItemID(index);

    GetSubsystem<UI>()->GetTBIDString(id, _id);

    return _id;

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

void UISelectList::ScrollToSelectedItem()
{
    if (!widget_)
        return;

    ((TBSelectList*)widget_)->ScrollToSelectedItem();

}

void UISelectList::HandleUIUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!widget_)
        return;

    // if we have a drag and drop item, auto scroll if top/bottom

    UIDragDrop* dragDrop = GetSubsystem<UIDragDrop>();

    if (dragDrop->GetDraggingObject())
    {
        TBSelectList* select = (TBSelectList*) widget_;
        Input* input = GetSubsystem<Input>();
        IntVector2 pos = input->GetMousePosition();
        select->ConvertFromRoot(pos.x_, pos.y_);

        if ((select->GetHitStatus(pos.x_, pos.y_) != WIDGET_HIT_STATUS_NO_HIT))
        {

            // Adjust speed based on pixel distance from top and bottom
            int value = pos.y_;

            if (value > 16)
                value = select->GetRect().h - pos.y_;

            if (value > 16)
                return;

            int speed = 0;

            if (value <= 16)
                speed = -2;
            if (value < 8)
                speed = -4;

            if (pos.y_ > 16)
                speed = -speed;

            if (speed)
                select->GetScrollContainer()->ScrollBy(0, speed);

        }

    }

}

bool UISelectList::OnEvent(const tb::TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_POINTER_DOWN)
    {
        GetTBSelectList()->SetFocus(WIDGET_FOCUS_REASON_POINTER);
    }
    if (ev.type == EVENT_TYPE_POINTER_MOVE)
    {
        UIDragDrop* dragDrop = GetSubsystem<UIDragDrop>();
        //if return true, then scroll event will be controlled by that widget itself
        if (dragDrop->GetDraggingObject())
            return true;
    }
    return UIWidget::OnEvent(ev);
}

void UISelectList::SelectNextItem()
{
    if (!widget_)
        return;
    
    ((TBSelectList*)widget_)->ChangeValue(TB_KEY_DOWN);
}

void UISelectList::SelectPreviousItem()
{
    if (!widget_)
        return;

    ((TBSelectList*)widget_)->ChangeValue(TB_KEY_UP);
}

void UISelectList::SetUIListView(bool value)
{
    if (!widget_)
        return;

    ((TBSelectList*)widget_)->SetUIListView(value);

}


}
