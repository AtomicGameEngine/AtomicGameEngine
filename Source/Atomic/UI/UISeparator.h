

#pragma once

#include "UIWidget.h"

namespace Atomic
{

class UISeparator : public UIWidget
{
    OBJECT(UISeparator)

public:

    UISeparator(Context* context, bool createWidget = true);
    virtual ~UISeparator();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}

