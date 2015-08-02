
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "UIEvents.h"
#include "UI.h"
#include "UIButton.h"

using namespace tb;

namespace Atomic
{

UIButton::UIButton(Context* context, bool createWidget) : UIWidget(context, false)
{
    if (createWidget)
    {
        widget_ = new TBButton();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }
}

UIButton::~UIButton()
{

}

void UIButton::SetSqueezable(bool value)
{
    if (!widget_)
        return;

    ((TBButton*)widget_)->SetSqueezable(value);
}

bool UIButton::OnEvent(const tb::TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK) 
	{
        String text = GetText();
        if (text.StartsWith("http://") || text.StartsWith("https://")) 
		{
            FileSystem* fileSystem = GetSubsystem<FileSystem>();
            fileSystem->SystemOpen(text);
        }
    }
    return UIWidget::OnEvent(ev);
}

}
