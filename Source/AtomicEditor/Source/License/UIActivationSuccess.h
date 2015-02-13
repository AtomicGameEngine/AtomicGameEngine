// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

// BEGIN LICENSE MANAGEMENT

#pragma once

#include "UI/Modal/UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class UIActivationSuccess: public UIModalOpWindow
{
    OBJECT(UIActivationSuccess);

public:

    UIActivationSuccess(Context* context);
    virtual ~UIActivationSuccess();
    bool OnEvent(const TBWidgetEvent &ev);

private:


};

}

// END LICENSE MANAGEMENT
