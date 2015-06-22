
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_window.h>

#include "../IO/Log.h"

#include "UI.h"
#include "UIEvents.h"
#include "UIWindow.h"

using namespace tb;

namespace Atomic
{

UIWindow::UIWindow(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBWindow();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

void UIWindow::SetSettings(unsigned settings)
{
    if (!widget_)
        return;

    ((TBWindow*)widget_)->SetSettings((WINDOW_SETTINGS) settings);
}

void UIWindow::ResizeToFitContent()
{
    if (!widget_)
        return;

    ((TBWindow*)widget_)->ResizeToFitContent();

}

void UIWindow::Close()
{
    if (!widget_)
        return;

    ((TBWindow*)widget_)->Close();

}


void UIWindow::AddChild(UIWidget *child)
{
    if (!widget_ || !child->GetInternalWidget())
        return;

    UIWidget::AddChild(child);

    // this is to get proper padding, this may cause problems
    // as this is called from the widget_reader, and not as part of
    // AddChild.  This may also need to be called from other widgets
    // that have padding, etc
    widget_->OnInflateChild(child->GetInternalWidget());

}

UIWindow::~UIWindow()
{
}

bool UIWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
