
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_select.h>


#include "UI.h"
#include "UIEvents.h"
#include "UISelectDropdown.h"

using namespace tb;

namespace Atomic
{

UISelectDropdown::UISelectDropdown(Context* context, bool createWidget) : UIButton(context, false)
{
    if (createWidget)
    {
        widget_ = new TBSelectDropdown();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UISelectDropdown::~UISelectDropdown()
{

}

void UISelectDropdown::SetSource(UISelectItemSource* source)
{
    if (!widget_)
        return;

    ((TBSelectDropdown*)widget_)->SetSource(source ? source->GetTBItemSource() : NULL);

}

bool UISelectDropdown::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIButton::OnEvent(ev);
}

}
