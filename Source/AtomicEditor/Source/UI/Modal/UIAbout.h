// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "UI/Modal/UIModalOps.h"

#include <TurboBadger/tb_widgets_common.h>

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

    void GenerateAboutText(String& text);


};

}

