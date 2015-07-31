
#pragma once

#include <TurboBadger/tb_select_item.h>
#include "UIWidget.h"

namespace Atomic
{

class UISelectItemSource;

class UIMenuWindow : public UIWidget
{
    OBJECT(UIMenuWindow)

public:

    UIMenuWindow(Context* context, UIWidget* target, const String& id);
    virtual ~UIMenuWindow();

    void Show(UISelectItemSource* source, int x = -1, int y = -1);

    void Close();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

    tb::TBSelectItemSource* source_;

};

}
