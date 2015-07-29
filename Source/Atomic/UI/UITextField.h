
#pragma once

#include "UIWidget.h"

namespace Atomic
{

/// TB_TEXT_ALIGN specifies horizontal text alignment
enum UI_TEXT_ALIGN
{
    UI_TEXT_ALIGN_LEFT = tb::TB_TEXT_ALIGN_LEFT,
    UI_TEXT_ALIGN_RIGHT = tb::TB_TEXT_ALIGN_RIGHT,
    UI_TEXT_ALIGN_CENTER = tb::TB_TEXT_ALIGN_CENTER
};

class UITextField : public UIWidget
{
    OBJECT(UITextField)

public:

    UITextField(Context* context, bool createWidget = true);
    virtual ~UITextField();

    void SetTextAlign(UI_TEXT_ALIGN align);

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
