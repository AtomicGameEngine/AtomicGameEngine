
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <Atomic/IO/Log.h>

#include "UIEvents.h"
#include "UI.h"
#include "UIScrollContainer.h"

using namespace tb;

namespace Atomic
{

UIScrollContainer::UIScrollContainer(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBScrollContainer();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIScrollContainer::~UIScrollContainer()
{

}

void UIScrollContainer::SetScrollMode(UI_SCROLL_MODE mode)
{
    if (!widget_)
        return;

    ((TBScrollContainer*) widget_)->SetScrollMode((tb::SCROLL_MODE) mode);

}

void UIScrollContainer::SetAdaptToContentSize(bool adapt)
{
    if (!widget_)
        return;

    ((TBScrollContainer*) widget_)->SetAdaptToContentSize(adapt);

}

bool UIScrollContainer::GetAdaptToContentSize()
{
    if (!widget_)
        return false;

    return ((TBScrollContainer*) widget_)->GetAdaptToContentSize();

}

void UIScrollContainer::SetAdaptContentSize(bool adapt)
{
    if (!widget_)
        return;

    ((TBScrollContainer*) widget_)->SetAdaptContentSize(adapt);

}

bool UIScrollContainer::GetAdaptContentSize()
{
    if (!widget_)
        return false;

    return ((TBScrollContainer*) widget_)->GetAdaptContentSize();

}


UI_SCROLL_MODE UIScrollContainer::GetScrollMode()
{
    if (!widget_)
        return UI_SCROLL_MODE_OFF;


    return (UI_SCROLL_MODE) ((TBScrollContainer*) widget_)->GetScrollMode();

}


bool UIScrollContainer::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
