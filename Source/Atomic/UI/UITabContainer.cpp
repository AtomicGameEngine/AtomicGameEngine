
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_tab_container.h>


#include "UI.h"
#include "UIEvents.h"
#include "UITabContainer.h"

using namespace tb;

namespace Atomic
{

UITabContainer::UITabContainer(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBTabContainer();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UITabContainer::~UITabContainer()
{

}

bool UITabContainer::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

}
