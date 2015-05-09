
#include <TurboBadger/tb_widgets.h>

#include "UI.h"
#include "UIView.h"

using namespace tb;

namespace Atomic
{

UIView::UIView(Context* context) : UIWidget(context, false)
{
    widget_ = new TBWidget();
    widget_->SetDelegate(this);
    GetSubsystem<UI>()->WrapWidget(this, widget_);

    UI* ui = GetSubsystem<UI>();

    TBRect rect = ui->GetRootWidget()->GetRect();
    widget_->SetSize(rect.w, rect.h);

    ui->GetRootWidget()->AddChild(widget_);

}

UIView::~UIView()
{

}


}
