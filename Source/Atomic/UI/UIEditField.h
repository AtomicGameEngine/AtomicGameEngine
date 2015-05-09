
#pragma once

#include "UIWidget.h"

namespace Atomic
{

enum TEXT_ALIGN
{
    TEXT_ALIGN_LEFT,		///< Aligned left
    TEXT_ALIGN_RIGHT,	///< Aligned right
    TEXT_ALIGN_CENTER	///< Aligned center
};


class UIEditField : public UIWidget
{
    OBJECT(UIEditField)

public:

    UIEditField(Context* context, bool createWidget = true);
    virtual ~UIEditField();

    void SetTextAlign(TEXT_ALIGN align);

    void SetWrapping(bool wrap);
    bool GetWrapping();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
