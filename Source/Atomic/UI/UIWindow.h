
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

    void SetSettings(unsigned settings);

    void ResizeToFitContent();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
