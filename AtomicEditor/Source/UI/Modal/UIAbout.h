// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include "UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class UIAbout: public UIModalOpWindow
{
    OBJECT(UIAbout);

public:

    UIAbout(Context* context);
    virtual ~UIAbout();
    bool OnEvent(const TBWidgetEvent &ev);

private:

};

}
