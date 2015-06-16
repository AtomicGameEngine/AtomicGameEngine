/*
#include <Atomic/IO/Log.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Input/Input.h>
#include <Atomic/UI/UIWidget.h>

#include <ToolCore/Assets/Asset.h>

#include <ThirdParty/TurboBadger/tb_widgets.h>

#include "AEDragAndDrop.h"

#ifdef ATOMIC_PLATFORM_OSX
#include "AEMacDragAndDrop.h"
#endif

namespace AtomicEditor
{

AEDragAndDrop::AEDragAndDrop(Context *context) : Object(context)
{
#ifdef ATOMIC_PLATFORM_OSX
    InitDragAndDrop(this);
#endif

    SubscribeToEvent(E_MOUSEMOVE, HANDLER(AEDragAndDrop,HandleMouseMove));
    SubscribeToEvent(E_MOUSEBUTTONUP, HANDLER(AEDragAndDrop,HandleMouseUp));
    SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(AEDragAndDrop,HandleMouseDown));

}

void AEDragAndDrop::HandleMouseDown(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseMove;

    Input* input = GetSubsystem<Input>();

    if (!input->IsMouseVisible())
        return;

    if (eventData[P_BUTTONS].GetUInt() & MOUSEB_LEFT)
    {
        // see if we have a widget
        if (tb::TBWidget::hovered_widget && tb::TBWidget::hovered_widget->GetDelegate())
        {
            UIWidget* widget = (UIWidget*) tb::TBWidget::hovered_widget->GetDelegate();
            dragObject_ = widget->GetDragObject();

            if (dragObject_ && dragObject_->GetType() == ToolCore::Asset::GetTypeStatic())
            {
                ToolCore::Asset* asset = (ToolCore::Asset*) dragObject_.Get();
                LOGINFOF("Drag Asset: %s", asset->GetPath().CString());

                #ifdef ATOMIC_PLATFORM_OSX
                StartDragAndDrop();
                #endif

            }
        }
    }
}

void AEDragAndDrop::HandleMouseUp(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseMove;

    Input* input = GetSubsystem<Input>();

    if (!input->IsMouseVisible())
        return;

    if (eventData[P_BUTTONS].GetUInt() & MOUSEB_LEFT)
    {
        dragObject_ = 0;
    }

}

void AEDragAndDrop::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseMove;

    Input* input = GetSubsystem<Input>();

    if (!input->IsMouseVisible())
        return;

    if (eventData[P_BUTTONS].GetUInt() & MOUSEB_LEFT)
    {
        int x = eventData[P_X].GetInt();
        int y = eventData[P_Y].GetInt();
    }
}

void AEDragAndDrop::BeginDrag()
{
    dragFilenames_.Clear();
}

void AEDragAndDrop::AddDragFilename(const String& filename)
{
    dragFilenames_.Push(filename);
}

void AEDragAndDrop::ConcludeDrag()
{
    for (unsigned i = 0; i < dragFilenames_.Size(); i++)
    {
        VariantMap eventData;
        eventData[DragAndDrop::P_FILENAME] = dragFilenames_[i];
        SendEvent(E_DRAGANDDROP, eventData);
    }

    dragFilenames_.Clear();
}

}

*/
