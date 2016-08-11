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
#include <TurboBadger/tb_menu_window.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIWidget.h"
#include "UISelectItem.h"
#include "UIMenuWindow.h"

using namespace tb;

namespace Atomic
{

UIMenuWindow::UIMenuWindow(Context* context, UIWidget* target, const String& id) : UIWidget(context, false)
  , source_(0)
{
    widget_ = new TBMenuWindow(target->GetInternalWidget(), TBID(id.CString()));
    widget_->SetDelegate(this);
    GetSubsystem<UI>()->WrapWidget(this, widget_);

}

UIMenuWindow::~UIMenuWindow()
{
    if (source_)
        delete source_;
}

void UIMenuWindow::Show(UISelectItemSource* source, int x, int y)
{
    if (source_)
        delete source_;

    source_ = source->GetTBItemSource();

    // get rid of scrollbars for menu popups, reconfigure menu container
    TBScrollContainer *mys = ((TBMenuWindow*)widget_ )->GetList()->GetScrollContainer();
    mys->SetScrollMode(SCROLL_MODE_OFF);
    mys->SetAdaptToContentSize(true);
    mys->SetAdaptContentSize(false);

    if (x != -1 && y != -1)
    {
        ((TBMenuWindow*)widget_)->Show(source_, TBPopupAlignment(TBPoint(x, y)));
    }
    else
    {
        ((TBMenuWindow*)widget_)->Show(source_, TBPopupAlignment());
    }

    TBMenuWindow* src = (TBMenuWindow*)widget_; // get rid of scrollbars for menu popups, fix size
    TBSelectList* list = ((TBMenuWindow*)widget_ )->GetList();
    int num = list->GetNumItems() - 1;
    int sheight = list->GetItemWidget(num)->GetRect().y + list->GetItemWidget(num)->GetRect().h + 4;
    if ( src->GetRect().h != sheight )
        src->SetSize(src->GetRect().w, sheight);

}

bool UIMenuWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

void UIMenuWindow::Close()
{
    if (!widget_)
        return;

    ((TBMenuWindow*)widget_)->Close();

}

}
