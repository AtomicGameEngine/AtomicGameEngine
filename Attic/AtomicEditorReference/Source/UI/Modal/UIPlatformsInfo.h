// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class PlatformsInfo: public UIModalOpWindow
{
    OBJECT(PlatformsInfo);

public:

    PlatformsInfo(Context* context);
    virtual ~PlatformsInfo();
    bool OnEvent(const TBWidgetEvent &ev);

private:

};

}
