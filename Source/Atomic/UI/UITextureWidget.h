
#pragma once

#include "UIWidget.h"

namespace Atomic
{

class Texture;
class UIRenderer;

/// A widget that can render a Texture2D, so the image data
/// doesn't need to be loaded 2x (once for Texture2D and once for say a UIImageWidget)
class UITextureWidget : public UIWidget
{
    friend class TBTextureWidget;

    OBJECT(UITextureWidget)

public:

    UITextureWidget(Context* context, bool createWidget = true);
    virtual ~UITextureWidget();

    void SetTexture(Texture *texture);
    Texture* GetTexture();

protected:

    virtual bool OnEvent(const tb::TBWidgetEvent &ev);

private:

    SharedPtr<Texture> texture_;

    UIRenderer* renderer_;

};

}
