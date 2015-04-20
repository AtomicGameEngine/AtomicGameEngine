
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/image/tb_image_widget.h>

#include "UIEvents.h"
#include "UIWidget.h"
#include "UILayout.h"

using namespace tb;

namespace Atomic
{

UILayout::UILayout(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBLayout();
        widget_->SetDelegate(this);
    }
}

UILayout::~UILayout()
{
}

bool UILayout::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
