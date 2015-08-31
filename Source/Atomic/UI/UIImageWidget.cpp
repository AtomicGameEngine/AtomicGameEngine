
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/image/tb_image_widget.h>

#include <Atomic/IO/Log.h>

#include "UI.h"
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
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIImageWidget::~UIImageWidget()
{
}

int UIImageWidget::GetImageWidth() const
{
    if (!widget_)
        return 0;

    return ((TBImageWidget*) widget_)->GetImageWidth();

}

int UIImageWidget::GetImageHeight() const
{
    if (!widget_)
        return 0;

    return ((TBImageWidget*) widget_)->GetImageHeight();

}

void UIImageWidget::SetImage(const String& imagePath)
{
    if (!widget_)
        return;

    ((TBImageWidget*) widget_)->SetImage(imagePath.CString());

}

bool UIImageWidget::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
