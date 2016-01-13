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

#include "UI.h"
#include "UIView.h"

using namespace tb;

namespace Atomic
{

UIView::UIView(Context* context) : UIWidget(context, false)
{
    widget_ = new TBWidget();
    widget_->SetDelegate(this);

    // Set gravity all so we resize correctly
    widget_->SetGravity(WIDGET_GRAVITY_ALL);

    UI* ui = GetSubsystem<UI>();
    ui->WrapWidget(this, widget_);

    // Set initial size for view
    TBRect rect = ui->GetRootWidget()->GetRect();
    widget_->SetSize(rect.w, rect.h);

    // add to the root widget
    ui->GetRootWidget()->AddChild(widget_);

}

UIView::~UIView()
{

}


}
