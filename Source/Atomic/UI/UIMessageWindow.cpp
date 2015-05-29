
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_message_window.h>

#include "../IO/Log.h"

#include "UI.h"
#include "UIEvents.h"
#include "UIMessageWindow.h"

using namespace tb;

namespace Atomic
{

UIMessageWindow::UIMessageWindow(Context* context, UIWidget* target, const String& id, bool createWidget) : UIWindow(context, false)
{
    if (createWidget)
    {
        widget_ = new TBMessageWindow(target ? target->GetInternalWidget() : 0, TBIDC(id.CString()));
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIMessageWindow::~UIMessageWindow()
{

}

void UIMessageWindow::Show(const String &title, const String &message, int width, int height)
{
    if (!widget_)
        return;

    TBMessageWindowSettings settings;
    settings.styling = true;
    settings.dimmer = true;

    ((TBMessageWindow*)widget_)->Show(title.CString(), message.CString(), &settings, width, height);

}

bool UIMessageWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWindow::OnEvent(ev);
}

}
