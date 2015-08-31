
#pragma once

#include "UIWidget.h"

namespace Atomic
{


class UIImageWidget : public UIWidget
{
    OBJECT(UIImageWidget)

public:

    UIImageWidget(Context* context, bool createWidget = true);
    virtual ~UIImageWidget();

    void SetImage(const String& imagePath);

    int GetImageWidth() const;
    int GetImageHeight() const;

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

};

}
