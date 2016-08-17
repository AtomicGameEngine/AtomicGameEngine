//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_editfield.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIEditField.h"

using namespace tb;

namespace Atomic
{

UIEditField::UIEditField(Context* context, bool createWidget) : UIWidget(context, false),
    firstFocusFlag_(false)
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

void UIEditField::Reformat(bool update_fragments)
{
    if (!widget_)
        return;

    TBEditField* w = (TBEditField*) widget_;
    w->Reformat(update_fragments);
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

void UIEditField::OnFocusChanged(bool focused)
{
    UIWidget::OnFocusChanged(focused);

    if (!widget_)
        return;

    // safe cast?
    TBEditField* w = (TBEditField*) widget_;

    TBStyleEdit* styleEdit = w->GetStyleEdit();
    if (styleEdit != NULL)
    {
        if (focused)
        {
            if (!w->GetMultiline())
                styleEdit->selection.SelectAll();
            firstFocusFlag_ = true;
        }
        else
        {
            styleEdit->selection.SelectNothing();
        }
    }

}

bool UIEditField::OnEvent(const tb::TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CUSTOM && ev.ref_id == TBIDC("edit_complete"))
    {
        VariantMap eventData;
        eventData[UIWidgetEditComplete::P_WIDGET] = this;
        SendEvent(E_UIWIDGETEDITCOMPLETE, eventData);
        return true;
    }
    else if (ev.type == EVENT_TYPE_POINTER_DOWN)
    {
        // Select the entire value in the field when it is selected
        if (widget_ && firstFocusFlag_)
        {
            firstFocusFlag_ = false;

            // safe cast?
            TBEditField* w = (TBEditField*) widget_;

            TBStyleEdit* styleEdit = w->GetStyleEdit();
            if (styleEdit != NULL && !w->GetMultiline())
            {
                styleEdit->selection.SelectAll();
            }
        }
    }

    return UIWidget::OnEvent(ev);
}

}
