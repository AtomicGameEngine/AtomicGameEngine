
#pragma once

#include "UIWidget.h"

namespace Atomic
{

class UITabContainer : public UIWidget
{
    OBJECT(UITabContainer)

public:

    UITabContainer(Context* context, bool createWidget = true);
    virtual ~UITabContainer();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
