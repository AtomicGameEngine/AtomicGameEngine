
#pragma once

#include "UIWindow.h"

namespace Atomic
{

class UIMessageWindow : public UIWindow
{
    OBJECT(UIMessageWindow)

public:

    UIMessageWindow(Context* context, UIWidget* target, const String& id, bool createWidget = true);
    virtual ~UIMessageWindow();

    void Show(const String& title, const String& message, int width, int height);

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
