
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UIWindow : public UIWidget
{
    OBJECT(UIWindow)

public:

    UIWindow(Context* context, bool createWidget = true);
    virtual ~UIWindow();

    void ResizeToFitContent();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
