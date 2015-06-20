
#pragma once

#include "UIWidget.h"

namespace Atomic
{

class UIContainer : public UIWidget
{
    OBJECT(UIContainer)

public:

    UIContainer(Context* context, bool createWidget = true);
    virtual ~UIContainer();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
