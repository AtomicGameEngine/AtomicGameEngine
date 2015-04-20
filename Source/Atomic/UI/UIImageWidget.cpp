
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/image/tb_image_widget.h>

#include <Atomic/IO/Log.h>

#include "UIEvents.h"
#include "UIWidget.h"
#include "UIImageWidget.h"

using namespace tb;

namespace Atomic
{

UIImageWidget::UIImageWidget(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBImageWidget();
        widget_->SetDelegate(this);
    }
}

UIImageWidget::~UIImageWidget()
{
    LOGINFOF("Hey!");
}

bool UIImageWidget::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
