//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <ThirdParty/TurboBadger/tb_widgets.h>

#include "../IO/Log.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"

#include "UI.h"
#include "UIEvents.h"
#include "UIWidget.h"
#include "UILayout.h"
#include "UIFontDescription.h"
#include "UITextField.h"
#include "UIImageWidget.h"
#include "UISelectList.h"
#include "UIDragDrop.h"
#include "UIDragObject.h"

#ifdef ATOMIC_PLATFORM_OSX
#include "UIDragDropMac.h"
#elif ATOMIC_PLATFORM_WINDOWS
#include "UIDragDropWindows.h"
#else
namespace Atomic { void InitDragAndDrop(UIDragDrop *dragAndDrop) {} }
#endif

using namespace tb;

namespace Atomic
{

UIDragDrop::UIDragDrop(Context* context) : Object(context)
{
    SubscribeToEvent(E_MOUSEMOVE, ATOMIC_HANDLER(UIDragDrop,HandleMouseMove));
    SubscribeToEvent(E_MOUSEBUTTONUP, ATOMIC_HANDLER(UIDragDrop,HandleMouseUp));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, ATOMIC_HANDLER(UIDragDrop,HandleMouseDown));

    SharedPtr<UIFontDescription> fd(new UIFontDescription(context));
    fd->SetId("Vera");
    fd->SetSize(12);

    dragText_ = new UITextField(context);
    dragText_->SetFontDescription(fd);
    dragText_->SetGravity(UI_GRAVITY_TOP);

    dragLayout_ = new UILayout(context);
    dragLayout_->AddChild(dragText_);

    dragLayout_->SetLayoutSize(UI_LAYOUT_SIZE_PREFERRED);
    dragLayout_->SetVisibility(UI_WIDGET_VISIBILITY_GONE);

    // put into hierarchy so we aren't pruned
    TBWidget* root = GetSubsystem<UI>()->GetRootWidget();
    root->AddChild(dragLayout_->GetInternalWidget());

    InitDragAndDrop(this);

}

UIDragDrop::~UIDragDrop()
{

}

void UIDragDrop::DragEnd()
{
    SharedPtr<UIDragObject> dragObject = dragObject_;
    SharedPtr<UIWidget> currentTargetWidget(currentTargetWidget_);
    SharedPtr<UIWidget> dragSourceWidget(dragSourceWidget_);

    // clean up
    currentTargetWidget_ = 0;
    dragObject_ = 0;
    dragSourceWidget_ = 0;
    dragLayout_->SetVisibility(UI_WIDGET_VISIBILITY_GONE);

    if (currentTargetWidget.Null())
    {
        return;
    }

    VariantMap dropData;
    dropData[DragEnded::P_TARGET] = currentTargetWidget;
    dropData[DragEnded::P_DRAGOBJECT] = dragObject;
    currentTargetWidget->SendEvent(E_DRAGENDED, dropData);
}

void UIDragDrop::HandleMouseDown(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseButtonDown;

    Input* input = GetSubsystem<Input>();

    if (!input->IsMouseVisible())
        return;

    if ((eventData[P_BUTTONS].GetUInt() & MOUSEB_LEFT) && TBWidget::hovered_widget)
    {
        // see if we have a widget with a drag object

        TBWidget* tbw = TBWidget::hovered_widget;
        UIWidget* widget = nullptr;

        while(tbw)
        {
            if (tbw->GetDelegate())
            {
                widget = (UIWidget*) tbw->GetDelegate();

                if (widget->GetDragObject())
                {
                    // TODO: check if we're in widget bounds
                    // this is going to need to be updated for drag/drop multiselect
                    break;
                }

                widget = nullptr;
            }

            tbw = tbw->GetParent();
        }

        if (!widget)
            return;

        currentTargetWidget_ = widget;
        dragSourceWidget_ = widget;
        mouseDownPosition_ = input->GetMousePosition();

    }

}

void UIDragDrop::HandleMouseUp(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseButtonUp;

    if (dragObject_.Null())
    {
        dragSourceWidget_ = 0;
        currentTargetWidget_ = 0;
        return;
    }

    if (!(eventData[P_BUTTON].GetInt() ==  MOUSEB_LEFT))
        return;

    DragEnd();

}

void UIDragDrop::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
    if (dragObject_.Null() && dragSourceWidget_.Null())
        return;

    if (dragObject_.Null())
    {
        dragObject_ = dragSourceWidget_->GetDragObject();

        if (dragObject_.Null())
        {
            dragSourceWidget_ = 0;
            return;
        }

    }

    using namespace MouseMove;

    int x = eventData[P_X].GetInt();
    int y = eventData[P_Y].GetInt();

    // tolerance to 8 pixels to start drag/drop operation
    IntVector2 mousePos(x, y);
    mousePos -= mouseDownPosition_;
    if (Abs(mousePos.x_) < 8 && Abs(mousePos.y_) < 8)
        return;

    // initialize if necessary
    if (dragLayout_->GetVisibility() == UI_WIDGET_VISIBILITY_GONE)
    {
        dragLayout_->GetInternalWidget()->SetZ(WIDGET_Z_TOP);
        dragLayout_->SetVisibility(UI_WIDGET_VISIBILITY_VISIBLE);
        dragText_->SetText(dragObject_->GetText());

        UIPreferredSize* sz = dragLayout_->GetPreferredSize();
        dragLayout_->SetRect(IntRect(0, 0, sz->GetMinWidth(), sz->GetMinHeight()));
    }

    // see if we have a widget
    TBWidget* tbw = TBWidget::hovered_widget;

    while(tbw && !tbw->GetDelegate())
    {
        tbw = tbw->GetParent();
    }

    if (!tbw || !tbw->GetParent())
        return;

    UIWidget* hoverWidget = (UIWidget*) tbw->GetDelegate();

    if (!hoverWidget->GetInternalWidget())
        return;

    if (hoverWidget != currentTargetWidget_)
    {
        if (currentTargetWidget_)
        {
            VariantMap exitData;
            exitData[DragExitWidget::P_WIDGET] = currentTargetWidget_;
            exitData[DragExitWidget::P_DRAGOBJECT] = dragObject_;
            currentTargetWidget_->SendEvent(E_DRAGEXITWIDGET, exitData);
        }

        currentTargetWidget_ = hoverWidget;

        VariantMap enterData;
        enterData[DragEnterWidget::P_WIDGET] = currentTargetWidget_;
        enterData[DragEnterWidget::P_DRAGOBJECT] = dragObject_;
        currentTargetWidget_->SendEvent(E_DRAGENTERWIDGET, enterData);

    }

    dragLayout_->SetPosition(x, y - 20);

}

void UIDragDrop::FileDragEntered()
{
    dragObject_ = new UIDragObject(context_);
    //dragObject_->SetText("Files...");
}

void UIDragDrop::FileDragAddFile(const String& filename)
{
    dragObject_->AddFilename(filename);
}

void UIDragDrop::FileDragConclude()
{
    DragEnd();
}


}
