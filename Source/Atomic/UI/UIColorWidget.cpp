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
#include <TurboBadger/tb_atomic_widgets.h>

#include <Atomic/IO/Log.h>
#include <Atomic/Graphics/Texture.h>

#include "UIEvents.h"
#include "UI.h"
#include "UIColorWidget.h"

using namespace tb;

namespace Atomic
{

UIColorWidget::UIColorWidget(Context* context, bool createWidget) : UIWidget(context, false)
{
    UI* ui = GetSubsystem<UI>();

    if (createWidget)
    {
        widget_ = new TBColorWidget();
        widget_->SetDelegate(this);
        ui->WrapWidget(this, widget_);
    }

}

UIColorWidget::~UIColorWidget()
{
}

bool UIColorWidget::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

void UIColorWidget::SetColorString( const String &name )
{
     if (!widget_)
        return;
    ((TBColorWidget*) widget_)->SetColor(name.CString());
}

void UIColorWidget::SetColor(const Color& color)
{
    if (!widget_)
        return;

    ((TBColorWidget*)widget_)->SetColor(color.r_ * 255.0f, color.g_ * 255.0f, color.b_ * 255.0f, color.a_ * 255.0f);
}

Color UIColorWidget::GetColor() const
{
    if (!widget_)
        return Color::BLACK;

    const TBColor& color = ((TBColorWidget*)widget_)->GetColor();
    float alpha = ((TBColorWidget*)widget_)->GetAlpha();

    return Color(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, alpha / 255.0f);

}

void UIColorWidget::SetAlpha ( float value )
{
     if (!widget_)
        return;
    ((TBColorWidget*) widget_)->SetAlpha(value);
}

}
