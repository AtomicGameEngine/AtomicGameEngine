
#pragma once

#include "UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class UIBuild: public UIModalOpWindow
{
    OBJECT(UIBuild);

public:

    UIBuild(Context* context);
    virtual ~UIBuild();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    TBTextField* buildPathField_;

};

}
