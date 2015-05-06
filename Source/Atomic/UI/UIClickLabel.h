
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UIClickLabel : public UIWidget
{
    OBJECT(UIClickLabel)

public:

    UIClickLabel(Context* context, bool createWidget = true);
    virtual ~UIClickLabel();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
