
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
    if (ev.type == EVENT_TYPE_CLICK)
    {        
        if (ev.target == widget_) // button clicked itself
        {
            VariantMap eventData;
            ConvertEvent(this, ev, eventData);
            SendEvent(E_WIDGETEVENT, eventData);

            // this is catching the window close button
            return true;
        }

        if (ev.target && ev.target->GetID() == TBID("__popup-menu"))
        {
            // popup menu

            if (JSGetHeapPtr())
            {
                VariantMap eventData;
                eventData[PopupMenuSelect::P_BUTTON] = this;
                eventData[PopupMenuSelect::P_REFID] = (unsigned) ev.ref_id;
                SendEvent(E_POPUPMENUSELECT, eventData);
            }

            return true;
        }
    }

    return false;
}

}
