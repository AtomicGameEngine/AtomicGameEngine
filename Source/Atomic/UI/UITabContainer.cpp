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
#include <TurboBadger/tb_tab_container.h>


#include "UI.h"
#include "UIEvents.h"
#include "UITabContainer.h"

using namespace tb;

namespace Atomic
{

UITabContainer::UITabContainer(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBTabContainer();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UITabContainer::~UITabContainer()
{

}

UIWidget* UITabContainer::GetCurrentPageWidget()
{
    if (!widget_)
        return 0;

    TBWidget* w = ((TBTabContainer*)widget_)->GetCurrentPageWidget();

    return GetSubsystem<UI>()->WrapWidget(w);
}

int UITabContainer::GetNumPages()
{
    if (!widget_)
        return 0;

    return ((TBTabContainer*)widget_)->GetNumPages();

}

UILayout* UITabContainer::GetTabLayout()
{
    if (!widget_)
        return 0;

    TBLayout* layout = ((TBTabContainer*)widget_)->GetTabLayout();

    if (!layout)
        return 0;

    UI* ui = GetSubsystem<UI>();
    return (UILayout*) ui->WrapWidget(layout);
}

void UITabContainer::SetCurrentPage(int page)
{
    if (!widget_)
        return;

    ((TBTabContainer*)widget_)->SetCurrentPage(page);

}

bool UITabContainer::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
