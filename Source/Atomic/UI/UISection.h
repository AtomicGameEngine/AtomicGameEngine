
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UISection : public UIWidget
{
    OBJECT(UISection)

public:

    UISection(Context* context, bool createWidget = true);
    virtual ~UISection();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
