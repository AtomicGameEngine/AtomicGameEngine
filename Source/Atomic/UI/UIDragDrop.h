
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

class UIWidget;
class UIImageWidget;
class UITextField;

/// UIDragDrop subsystem,

class UIDragDrop : public Object
{

    OBJECT(UIDragDrop);

public:
    /// Construct.
    UIDragDrop(Context* context);
    virtual ~UIDragDrop();

private:

    void HandleMouseDown(StringHash eventType, VariantMap& eventData);
    void HandleMouseUp(StringHash eventType, VariantMap& eventData);
    void HandleMouseMove(StringHash eventType, VariantMap& eventData);

    SharedPtr<UIWidget> dragWidget_;
    SharedPtr<UIImageWidget> dragImage_;
    SharedPtr<UITextField> dragText_;

    SharedPtr<Object> dragObject_;

};


}
