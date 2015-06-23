
#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_menu_window.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIWidget.h"
#include "UISelectItem.h"
#include "UIMenuWindow.h"

using namespace tb;

namespace Atomic
{

UIMenuWindow::UIMenuWindow(Context* context, UIWidget* target, const String& id) : UIWidget(context, false)
  , source_(0)
{
    widget_ = new TBMenuWindow(target->GetInternalWidget(), TBID(id.CString()));    
    widget_->SetDelegate(this);
    GetSubsystem<UI>()->WrapWidget(this, widget_);

}

UIMenuWindow::~UIMenuWindow()
{
    if (source_)
        delete source_;
}

void UIMenuWindow::Show(UISelectItemSource* source)
{
    if (source_)
        delete source_;

    source_ = source->GetTBItemSource();

    ((TBMenuWindow*)widget_)->Show(source_, TBPopupAlignment());
}

bool UIMenuWindow::OnEvent(const tb::TBWidgetEvent &ev)
{
    return false;
}

void UIMenuWindow::Close()
{
    if (!widget_)
        return;

    ((TBMenuWindow*)widget_)->Close();

}

}
