
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>

#include <Atomic/IO/Log.h>

#include "UIEvents.h"
#include "UI.h"
#include "UIClickLabel.h"

using namespace tb;

namespace Atomic
{

UIClickLabel::UIClickLabel(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBClickLabel();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIClickLabel::~UIClickLabel()
{
}

bool UIClickLabel::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
