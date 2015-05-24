
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
