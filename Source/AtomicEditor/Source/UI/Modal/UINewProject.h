// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class UINewProject: public UIModalOpWindow
{
    OBJECT(UINewProject);

public:

    UINewProject(Context* context);
    virtual ~UINewProject();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    bool CreateEmptyProject(const String& projectPath, const String &filename);
    bool Create2DProject(const String& projectPath, const String &filename);
    bool Create3DProject(const String& projectPath, const String &filename);

};

}
