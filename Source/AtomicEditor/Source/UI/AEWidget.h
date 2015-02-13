// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_layout.h>

using namespace Atomic;
using namespace tb;

namespace AtomicEditor
{

class TBWidgetDelegate;

class AEWidget : public Object
{
    OBJECT(AEWidget);

protected:

    TBWidgetDelegate* delegate_;

public:

    /// Construct.
    AEWidget(Context* context);
    /// Destruct.
    ~AEWidget();

    TBWidgetDelegate* GetWidgetDelegate() { return delegate_; }

    virtual bool OnEvent(const TBWidgetEvent &ev) { return false; }

};

class TBWidgetDelegate : public tb::TBWidget
{
    AEWidget* aewidget_;

public:

    TBWidgetDelegate(AEWidget *aewidget);

    bool OnEvent(const TBWidgetEvent &ev);
};

}
