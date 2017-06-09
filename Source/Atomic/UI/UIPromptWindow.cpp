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

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIWindow.h"
#include "UIEditField.h"
#include "UISelectList.h"
#include "UIPromptWindow.h"

using namespace tb;

namespace Atomic
{

UIPromptWindow::UIPromptWindow(Context* context, UIWidget* target, const String& id, bool createWidget) : UIWindow(context, false)
{
    if (createWidget)
    {
        widget_ = new TBPromptWindow(target ? target->GetInternalWidget() : 0, TBIDC(id.CString()));
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIPromptWindow::~UIPromptWindow()
{
}

void UIPromptWindow::Show(const String &title, const String &message, const String &preset, int dimmer, int width, int height)
{
    if (!widget_)
        return;

    ((TBPromptWindow*)widget_)->Show(title.CString(), message.CString(), preset.CString(), dimmer, width, height);

}

bool UIPromptWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
 
    if ( ev.type == EVENT_TYPE_CLICK 
        && ( ev.ref_id == TBID("TBPromptWindow.ok") || ev.ref_id == TBID("TBPromptWindow.cancel") ))
    {
        UI* ui = GetSubsystem<UI>();
        VariantMap eventData;

        String title = "PromptWindow";
        TBStr tbtext;
        if(  widget_ && (TBWindow*)widget_->GetText(tbtext) )
            title = tbtext.CStr();

        eventData[UIPromptComplete::P_TITLE] = title;
        eventData[UIPromptComplete::P_SELECTED] = "";
        eventData[UIPromptComplete::P_REASON] = "CANCEL";

        if ( ev.ref_id == TBID("TBPromptWindow.ok") )
        {
            eventData[UIPromptComplete::P_REASON] = "OK";
            UIWidget *ewidget = GetEditWidget();
            if( ewidget) 
                eventData[UIPromptComplete::P_SELECTED] = ewidget->GetText();
        }
        ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
        SendEvent(E_UIPROMPTCOMPLETE, eventData);

        if (eventData[WidgetEvent::P_HANDLED].GetBool())
           return true;
    }

    return UIWindow::OnEvent(ev);
}

UIWidget* UIPromptWindow::GetEditWidget()
{
    if (!widget_)
        return 0;
    TBWidget* child = (TBWidget*)widget_->GetWidgetByIDAndType<TBEditField>(UIPROMPTEDITID);
    if (!child)
        return 0;
    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

}
