
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_toggle_container.h>

#include <Atomic/IO/Log.h>

#include "UIEvents.h"
#include "UI.h"
#include "UISection.h"

using namespace tb;

namespace Atomic
{

UISection::UISection(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBSection();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UISection::~UISection()
{

}

bool UISection::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
