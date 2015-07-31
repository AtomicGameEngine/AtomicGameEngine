
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

int UISelectList::GetValue()
{
    if (!widget_)
        return 0;

    return ((TBSelectList*)widget_)->GetValue();

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
    return false;
}

}
