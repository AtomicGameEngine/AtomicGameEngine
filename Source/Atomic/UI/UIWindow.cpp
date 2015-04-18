
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_window.h>

#include "../IO/Log.h"

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
    }
}

void UIWindow::ResizeToFitContent()
{
    if (!widget_)
        return;

    ((TBWindow*)widget_)->ResizeToFitContent();

}

UIWindow::~UIWindow()
{

}

bool UIWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

}
