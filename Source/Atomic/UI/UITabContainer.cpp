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
#include "UILayout.h"
#include "UIButton.h"

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


/// returns the current page number
int UITabContainer::GetCurrentPage()
{
    if (!widget_)
        return 0;

    return ((TBTabContainer*)widget_)->GetCurrentPage();

}

/// deletes a tab + page, returns true if successful
bool UITabContainer::DeletePage( int page )
{
    if (!widget_ || page < 0 || page > GetNumPages() - 1)
        return false;

    UILayout *uil = GetTabLayout();
    if (uil)
    {
        UIWidget* mytab = NULL;
        int nn=0; 
        for (UIWidget *child = uil->GetFirstChild(); child; child = child->GetNext())
            if (nn++ == page)
                mytab = child;
        if (mytab)
        {
            mytab->UnsubscribeFromAllEvents();
            uil->RemoveChild( mytab, true );
        }
    }

    UIWidget *pages = GetContentRoot(); 
    if (pages)
    {
        UIWidget* mypage = NULL;
        int nn=0; 
        for (UIWidget *child = pages->GetFirstChild(); child; child = child->GetNext())
            if (nn++ == page)
                mypage = child;
        if (mypage)
        {
            mypage->UnsubscribeFromAllEvents();
            pages->RemoveChild( mypage, true );
        }
    }
    
    Invalidate();

    // tab container "feature", can not set it to the page number that was removed.
    int num = 0;
    if ( page - 1 > 0 ) num = page - 1;
    SetCurrentPage(num);

    return true;
}

/// adds a tab + page from layout file
void UITabContainer::AddTabPageFile ( const String &tabname, const String &layoutFile, bool selecttab )
{
    UIButton* button = new UIButton(context_);
    button->SetText(tabname);
    button->SetId(tabname);
    UILayout *uil = GetTabLayout();
    if (uil && button)
        uil->AddChild(button);
    UILayout* layout = new UILayout(context_);
    layout->SetAxis(UI_AXIS_Y);
    layout->SetLayoutSize(UI_LAYOUT_SIZE_AVAILABLE); 
    layout->SetLayoutPosition(UI_LAYOUT_POSITION_GRAVITY); 
    layout->SetLayoutDistribution(UI_LAYOUT_DISTRIBUTION_AVAILABLE); 
    layout->Load (layoutFile);
    UIWidget *pages = GetContentRoot();
    if (pages && layout)
        pages->AddChild(layout);

    Invalidate();

    if (selecttab) 
        SetCurrentPage( GetNumPages() -1 );
}

/// adds a tab + page widget(s)
void UITabContainer::AddTabPageWidget ( const String &tabname, UIWidget *widget, bool selecttab ) 
{
    UIButton* button = new UIButton(context_);
    button->SetText(tabname);
    button->SetId(tabname);
    UILayout *uil = GetTabLayout();
    if (uil && button)
        uil->AddChild(button);
    UILayout* layout = new UILayout(context_);
    layout->SetAxis(UI_AXIS_Y);
    layout->SetLayoutSize(UI_LAYOUT_SIZE_AVAILABLE);
    layout->SetLayoutPosition(UI_LAYOUT_POSITION_GRAVITY);
    layout->SetLayoutDistribution(UI_LAYOUT_DISTRIBUTION_AVAILABLE);
    layout->AddChild(widget);
    UIWidget *pages = GetContentRoot();
    if (pages && layout)
        pages->AddChild(layout);

    Invalidate();

    if (selecttab) 
        SetCurrentPage( GetNumPages() -1 );
}

/// undocks the page into a window with the tab name, and removes the tab
void UITabContainer::UndockPage ( int page )
{
    if (!widget_)
        return;

    ((TBTabContainer*)widget_)->UndockPage(page);

    // tab container "feature", can not set it to the page number that was removed.
    int num = 0;
    if ( page - 1 > 0 ) num = page - 1;
    SetCurrentPage(num);

}

/// docks content from a UIDockWindow with specified title
bool UITabContainer::DockWindow ( String windowTitle )
{
    if (!widget_)
        return false;
    bool done = ((TBTabContainer*)widget_)->DockFromWindow(windowTitle.CString());
    if (done) 
        SetCurrentPage( GetNumPages() -1 );

   return done;
}

}
