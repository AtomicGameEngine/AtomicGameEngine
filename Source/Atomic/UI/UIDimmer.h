
#pragma once

#include "UIWidget.h"

namespace Atomic
{

class UIDimmer : public UIWidget
{
    OBJECT(UIDimmer)

public:

    UIDimmer(Context* context, bool createWidget = true);
    virtual ~UIDimmer();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
