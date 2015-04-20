
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UILayout : public UIWidget
{
    OBJECT(UILayout)

public:

    UILayout(Context* context, bool createWidget = true);
    virtual ~UILayout();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
