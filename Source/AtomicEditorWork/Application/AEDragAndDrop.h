
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

/// A file was drag-dropped into the application window.
EVENT(E_DRAGANDDROP, DragAndDrop)
{
    PARAM(P_FILENAME, FileName);            // String
}

class AEDragAndDrop : public Object
{

    OBJECT(AEDragAndDrop);

public:
    /// Construct.
    AEDragAndDrop(Context* context);

    void BeginDrag();
    void AddDragFilename(const String& filename);
    void ConcludeDrag();

private:

    Vector<String> dragFilenames_;

};

}
