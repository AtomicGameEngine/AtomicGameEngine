
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

UILayout::UILayout(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBLayout();
        widget_->SetDelegate(this);
        widget_->SetGravity(WIDGET_GRAVITY_ALL);
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

void UILayout::SetLayoutSize(unsigned size)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetLayoutSize((LAYOUT_SIZE) size);

}

void UILayout::SetAxis(unsigned axis)
{
    if (!widget_)
        return;

    ((tb::TBLayout*)widget_)->SetAxis((AXIS) axis);
}

void UILayout::SetLayoutDistribution(/* LAYOUT_DISTRIBUTION */ unsigned distribution)
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
