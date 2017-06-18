//
// Copyright (c) 2017, THUNDERBEAST GAMES LLC All rights reserved
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
#include <TurboBadger/tb_atomic_widgets.h>

#include <Atomic/IO/Log.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIPulldownMenu.h"

using namespace tb;

namespace Atomic
{

UIPulldownMenu::UIPulldownMenu(Context* context, bool createWidget) : UIButton(context, false)
{
    if (createWidget)
    {
        widget_ = new TBPulldownMenu();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIPulldownMenu::~UIPulldownMenu()
{

}

void UIPulldownMenu::SetSource(UISelectItemSource* source)
{
    if (!widget_)
        return;

    ((TBSelectDropdown*)widget_)->SetSource(source ? source->GetTBItemSource() : NULL);

}

const String& UIPulldownMenu::GetSelectedId()
{
    if (!widget_)
    {
        if (sid_.Length())
            sid_.Clear();
        return sid_;
    }

    UI* ui = GetSubsystem<UI>();
    ui->GetTBIDString(((TBPulldownMenu*)widget_)->GetValueID(), sid_);

    return sid_;

}
bool UIPulldownMenu::OnEvent(const tb::TBWidgetEvent &ev)
{

   return UIButton::OnEvent(ev);

}

}
