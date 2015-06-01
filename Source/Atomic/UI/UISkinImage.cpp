
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_editfield.h>

#include "UI.h"
#include "UIEvents.h"
#include "UISkinImage.h"

using namespace tb;

namespace Atomic
{

UISkinImage::UISkinImage(Context* context, const String& bitmapID, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        if (bitmapID.Length())
            widget_ = new TBSkinImage(TBIDC(bitmapID.CString()));
        else
            widget_ = new TBSkinImage();

        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UISkinImage::~UISkinImage()
{

}

bool UISkinImage::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

}
