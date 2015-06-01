
#pragma once

#include "UIWidget.h"

namespace Atomic
{

class UISkinImage : public UIWidget
{
    OBJECT(UISkinImage)

public:

    UISkinImage(Context* context, const String& bitmapID, bool createWidget = true);
    virtual ~UISkinImage();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
