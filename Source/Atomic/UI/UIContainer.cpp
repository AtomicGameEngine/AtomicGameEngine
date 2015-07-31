
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>

#include <Atomic/IO/Log.h>

#include "UIEvents.h"
#include "UI.h"
#include "UIContainer.h"

using namespace tb;

namespace Atomic
{

UIContainer::UIContainer(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBContainer();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIContainer::~UIContainer()
{

}

bool UIContainer::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
