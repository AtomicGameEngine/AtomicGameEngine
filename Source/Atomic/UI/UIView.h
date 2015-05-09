
#pragma once

#include "UIWidget.h"

namespace Atomic
{

/// a view off the root widget
class UIView : public UIWidget
{
    OBJECT(UIView)

public:

    UIView(Context* context);
    virtual ~UIView();

protected:

private:

};

}
