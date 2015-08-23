
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

void UIEditField::SetStyling(bool styling)
{
    if (!widget_)
        return;

    TBEditField* w = (TBEditField*) widget_;

    w->SetStyling(styling);

}

void UIEditField::SetMultiline(bool multiline)
{
    if (!widget_)
        return;

    TBEditField* w = (TBEditField*) widget_;

    w->SetMultiline(multiline);

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

void UIEditField::SetEditType(UI_EDIT_TYPE type)
{
    if (!widget_)
        return;

    // safe cast?
    TBEditField* w = (TBEditField*) widget_;

    w->SetEditType((tb::EDIT_TYPE) type);

}

void UIEditField::ScrollTo(int x, int y)
{
    if (!widget_)
        return;

    // safe cast?
    TBEditField* w = (TBEditField*) widget_;

    w->ScrollTo(x, y);

}

void UIEditField::AppendText(const String& text)
{
    if (!widget_)
        return;

    // safe cast?
    TBEditField* w = (TBEditField*) widget_;

    w->AppendText(text.CString());

}

void UIEditField::SetAdaptToContentSize(bool adapt)
{
    if (!widget_)
        return;

    TBEditField* w = (TBEditField*) widget_;

    w->SetAdaptToContentSize(adapt);

}

bool UIEditField::GetAdaptToContentSize() const
{
    if (!widget_)
        return false;

    // safe cast?
    TBEditField* w = (TBEditField*) widget_;

    return w->GetAdaptToContentSize();

}


void UIEditField::SetTextAlign(UI_TEXT_ALIGN align)
{
    if (!widget_)
        return;

    // safe cast?
    TBEditField* w = (TBEditField*) widget_;

    switch (align)
    {
        case UI_TEXT_ALIGN_CENTER:
            w->SetTextAlign(TB_TEXT_ALIGN_CENTER);
            break;
        case UI_TEXT_ALIGN_LEFT:
            w->SetTextAlign(TB_TEXT_ALIGN_LEFT);
            break;
        case UI_TEXT_ALIGN_RIGHT:
            w->SetTextAlign(TB_TEXT_ALIGN_RIGHT);
            break;
    }

}

bool UIEditField::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
