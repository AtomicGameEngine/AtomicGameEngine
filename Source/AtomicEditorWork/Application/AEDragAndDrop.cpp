
#include <Atomic/Graphics/Graphics.h>

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
