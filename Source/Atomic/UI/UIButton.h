
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UIButton : public UIWidget
{
    OBJECT(UIButton)

public:

    UIButton(Context* context, bool createWidget = true);
    virtual ~UIButton();

    void SetSqueezable(bool value);

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
