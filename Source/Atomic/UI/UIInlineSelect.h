
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UIInlineSelect : public UIWidget
{
    OBJECT(UIInlineSelect)

public:

    UIInlineSelect(Context* context, bool createWidget = true);
    virtual ~UIInlineSelect();

    void SetLimits(double minimum, double maximum);

    void SetEditFieldLayoutParams(UILayoutParams* params);

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
