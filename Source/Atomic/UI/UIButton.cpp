
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>

#include "UIEvents.h"
#include "UIButton.h"

using namespace tb;

namespace Atomic
{

UIButton::UIButton(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBButton();
        widget_->SetDelegate(this);
    }
}

UIButton::~UIButton()
{

}

bool UIButton::OnEvent(const tb::TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK && ev.target == widget_)
    {
        VariantMap eventData;
        ConvertEvent(this, ev, eventData);
        SendEvent(E_WIDGETEVENT, eventData);
        return true;
    }

    return false;
}

}
