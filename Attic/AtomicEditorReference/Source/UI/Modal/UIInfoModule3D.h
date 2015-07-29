// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class InfoModule3D : public UIModalOpWindow
{
    OBJECT(InfoModule3D);

public:

    InfoModule3D(Context* context, const String &exampleFolder, const String &exampleScreenshot);
    virtual ~InfoModule3D();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    String exampleFolder_;
    String exampleScreenshot_;
};

}
