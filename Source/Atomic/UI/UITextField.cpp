
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>

#include "UI.h"
#include "UIEvents.h"
#include "UITextField.h"

using namespace tb;

namespace Atomic
{

UITextField::UITextField(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBTextField();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UITextField::~UITextField()
{

}

void UITextField::SetTextAlign(UI_TEXT_ALIGN align)
{
    if (!widget_)
        return;

    ((TBTextField*)widget_)->SetTextAlign((TB_TEXT_ALIGN) align);

}

bool UITextField::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

}
