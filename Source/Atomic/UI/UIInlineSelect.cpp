
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_inline_select.h>

#include <Atomic/IO/Log.h>

#include "UIEvents.h"
#include "UI.h"
#include "UILayout.h"
#include "UIInlineSelect.h"

using namespace tb;

namespace Atomic
{

UIInlineSelect::UIInlineSelect(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBInlineSelect();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIInlineSelect::~UIInlineSelect()
{

}

void UIInlineSelect::SetEditFieldLayoutParams(UILayoutParams* params)
{
    if (!params || !widget_)
        return;

    ((TBInlineSelect*) widget_)->SetEditFieldLayoutParams(*(params->GetTBLayoutParams()));

}

void UIInlineSelect::SetLimits(double minimum, double maximum)
{
    if (!widget_)
        return;
    ((TBInlineSelect*) widget_)->SetLimits(minimum, maximum);

}

bool UIInlineSelect::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
