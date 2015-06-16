
#include <ThirdParty/TurboBadger/tb_widgets.h>

#include "../IO/Log.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"

#include "UI.h"
#include "UIWidget.h"
#include "UIFontDescription.h"
#include "UITextField.h"
#include "UIImageWidget.h"
#include "UISelectList.h"
#include "UIDragDrop.h"

using namespace tb;

namespace Atomic
{

UIDragDrop::UIDragDrop(Context* context) : Object(context)
{
    SubscribeToEvent(E_MOUSEMOVE, HANDLER(UIDragDrop,HandleMouseMove));
    SubscribeToEvent(E_MOUSEBUTTONUP, HANDLER(UIDragDrop,HandleMouseUp));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(UIDragDrop,HandleMouseDown));

    SharedPtr<UIFontDescription> fd(new UIFontDescription(context));
    fd->SetId("Vera");
    fd->SetSize(12);

    dragText_ = new UITextField(context);
    dragText_->SetFontDescription(fd);
    dragText_->SetText("This is a test!");
    dragText_->SetGravity(WIDGET_GRAVITY_TOP);

    UIPreferredSize* sz = dragText_->GetPreferredSize();
    dragText_->SetRect(IntRect(0, 0, sz->GetMinWidth(), sz->GetMinHeight()));

    dragWidget_ = new UIWidget(context);
    dragWidget_->AddChild(dragText_);

    sz = dragWidget_->GetPreferredSize();
    dragWidget_->SetRect(IntRect(0, 0, sz->GetMinWidth(), sz->GetMinHeight()));

    // put into hierarchy so we aren't pruned
    TBWidget* root = GetSubsystem<UI>()->GetRootWidget();
    root->AddChild(dragWidget_->GetInternalWidget());

}

UIDragDrop::~UIDragDrop()
{

}

void UIDragDrop::HandleMouseDown(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseButtonDown;

    Input* input = GetSubsystem<Input>();

    if (!input->IsMouseVisible())
        return;

    if ((eventData[P_BUTTONS].GetUInt() & MOUSEB_LEFT) && TBWidget::hovered_widget)
    {
        // see if we have a widget

        TBWidget* tbw = TBWidget::hovered_widget;

        while(tbw && !tbw->GetDelegate())
        {
            tbw = tbw->GetParent();
        }

        if (!tbw)
            return;

        UIWidget* widget = (UIWidget*) tbw->GetDelegate();

        if (widget->GetType() == UISelectList::GetTypeStatic())
        {
            // handle select drag
            LOGINFOF("DRAG Select: %s", widget->GetTypeName().CString());
        }
        else
        {
            SharedPtr<Object> dragObject(widget->GetDragObject());

            dragWidget_->GetInternalWidget()->SetZ(WIDGET_Z_TOP);

            if (dragObject.NotNull())
                LOGINFOF("DRAG WIDGET: %s Object: %s", widget->GetTypeName().CString(), dragObject->GetTypeName().CString());
            else
                LOGINFOF("DRAG WIDGET: %s Object: None", widget->GetTypeName().CString());
        }

    }

}

void UIDragDrop::HandleMouseUp(StringHash eventType, VariantMap& eventData)
{

}

void UIDragDrop::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseMove;

    Input* input = GetSubsystem<Input>();

    if (!input->IsMouseVisible())
        return;

    int x = eventData[P_X].GetInt();
    int y = eventData[P_Y].GetInt();

    dragWidget_->SetPosition(x, y - 20);

}

}
