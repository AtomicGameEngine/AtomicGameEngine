
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

class UIWidget;
class UILayout;
class UIImageWidget;
class UITextField;
class UIDragObject;

/// UIDragDrop subsystem
class UIDragDrop : public Object
{

    OBJECT(UIDragDrop);

public:
    /// Construct.
    UIDragDrop(Context* context);
    virtual ~UIDragDrop();

    void FileDragEntered();
    void FileDragAddFile(const String& filename);
    void FileDragConclude();

private:

    void HandleMouseDown(StringHash eventType, VariantMap& eventData);
    void HandleMouseUp(StringHash eventType, VariantMap& eventData);
    void HandleMouseMove(StringHash eventType, VariantMap& eventData);

    void DragEnd();

    // attached to mouse UI
    SharedPtr<UILayout> dragLayout_;
    SharedPtr<UIImageWidget> dragImage_;
    SharedPtr<UITextField> dragText_;

    SharedPtr<UIWidget> currentTargetWidget_;

    SharedPtr<UIDragObject> dragObject_;

    SharedPtr<UIDragObject> dragSelectObject_;

};


}
