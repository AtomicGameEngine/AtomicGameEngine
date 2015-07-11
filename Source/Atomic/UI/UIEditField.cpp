
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_editfield.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIEditField.h"

using namespace tb;

namespace Atomic
{

UIEditField::UIEditField(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBEditField();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIEditField::~UIEditField()
{

}

void UIEditField::SetReadOnly(bool readonly)
{
    if (!widget_)
        return;

    TBEditField* w = (TBEditField*) widget_;

    w->SetReadOnly(readonly);

}

void UIEditField::SetWrapping(bool wrap)
{
    if (!widget_)
        return;

    TBEditField* w = (TBEditField*) widget_;

    w->SetWrapping(wrap);
}

bool UIEditField::GetWrapping()
{
    if (!widget_)
        return false;

    TBEditField* w = (TBEditField*) widget_;

    return w->GetWrapping();

}

void UIEditField::SetTextAlign(TEXT_ALIGN align)
{
    if (!widget_)
        return;

    // safe cast?
    TBEditField* w = (TBEditField*) widget_;

    switch (align)
    {
        case TEXT_ALIGN_CENTER:
            w->SetTextAlign(TB_TEXT_ALIGN_CENTER);
            break;
        case TEXT_ALIGN_LEFT:
            w->SetTextAlign(TB_TEXT_ALIGN_LEFT);
            break;
        case TEXT_ALIGN_RIGHT:
            w->SetTextAlign(TB_TEXT_ALIGN_RIGHT);
            break;
    }

}

bool UIEditField::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
