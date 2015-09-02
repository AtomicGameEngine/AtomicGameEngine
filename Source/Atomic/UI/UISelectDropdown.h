
#pragma once

#include "UISelectItem.h"
#include "UIButton.h"

namespace Atomic
{

class UISelectDropdown : public UIButton
{
    OBJECT(UISelectDropdown)

public:

    UISelectDropdown(Context* context, bool createWidget = true);
    virtual ~UISelectDropdown();

    void SetSource(UISelectItemSource* source);

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
