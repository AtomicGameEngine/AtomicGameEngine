
#pragma once

#include "UIWidget.h"
#include "UISelectItem.h"

namespace tb
{
    class TBSelectList;
}

namespace Atomic
{

class UISelectList : public UIWidget
{
    OBJECT(UISelectList)

public:

    UISelectList(Context* context, bool createWidget = true);
    virtual ~UISelectList();

    void SetFilter(const String& filter);

    void SetSource(UISelectItemSource* source);
    void InvalidateList();

    void SetValue(int value);
    int GetValue();

    String GetHoverItemID();
    String GetSelectedItemID();

    tb::TBSelectList* GetTBSelectList();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};


}
