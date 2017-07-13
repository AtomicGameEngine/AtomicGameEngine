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
#include <TurboBadger/tb_window.h>

#include "../IO/Log.h"

#include "UI.h"
#include "UIEvents.h"
#include "UIDockWindow.h"

using namespace tb;

namespace Atomic
{

UIDockWindow::UIDockWindow(Context* context, bool createWidget, const String& title, UIWidget *contentptr, int minwidth, int minheight ) : UIWindow(context, false)
{
    if (createWidget)
    {
        tb:TBWidget *contents = contentptr->GetInternalWidget();
        widget_ = new TBDockWindow( title.CString(), contents, minwidth, minheight );
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIDockWindow::~UIDockWindow()
{
}

void UIDockWindow::SetDockOrigin( String dockid ) // where using the dock button returns the content to
{
    if (!widget_)
        return;

   ((TBDockWindow*)widget_)->SetDockOrigin(TBIDC(dockid.CString()));

}

UIWidget *UIDockWindow::GetDockContent()  // the content that is being swapped around
{
    if (!widget_)
        return NULL;

    TBWidget* content = ((TBDockWindow*)widget_)->GetDockContent();

    if (!content)
        return 0;

    UI* ui = GetSubsystem<UI>();

    return ui->WrapWidget(content);

}

bool UIDockWindow::HasDockContent() // is there content in the dockwindow
{
    if (!widget_)
        return false;

    return ((TBDockWindow*)widget_)->HasDockContent();
}

void UIDockWindow::Dock( UIWidget *target ) // move the dock window content to somewhere else
{
    if (!widget_)
        return;

    if (!target)
        return;

    ((TBDockWindow*)widget_)->Dock( target->GetInternalWidget() );
}

void UIDockWindow::Show( UIWidget *host, int xpos, int ypos ) // show the dock window
{
    if (!widget_)
        return;

    if (!host)
        return;

    ((TBDockWindow*)widget_)->Show( host->GetInternalWidget(), xpos, ypos );

}

bool UIDockWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}





}
