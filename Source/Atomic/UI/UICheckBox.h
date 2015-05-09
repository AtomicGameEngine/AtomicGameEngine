
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UICheckBox : public UIWidget
{
    OBJECT(UICheckBox)

public:

    UICheckBox(Context* context, bool createWidget = true);
    virtual ~UICheckBox();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
