// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/IO/Log.h>
#include <Atomic/UI/UI.h>

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_menu_window.h>

#include "UIMainFrame.h"

using namespace tb;


namespace AtomicEditor
{

TBWidgetDelegate::TBWidgetDelegate(AEWidget *aewidget)
{
    aewidget_ = aewidget;
}

bool TBWidgetDelegate::OnEvent(const TBWidgetEvent &ev)
{
    return aewidget_->OnEvent(ev);
}


AEWidget::AEWidget(Context* context) :
    Object(context)
{
    delegate_ = new TBWidgetDelegate(this);
}

AEWidget::~AEWidget()
{
    if (delegate_ && delegate_->GetParent())
    {
        delegate_->GetParent()->RemoveChild(delegate_);
        delete delegate_;
    }
}

}
