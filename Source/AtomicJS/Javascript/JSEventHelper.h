
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

class ATOMIC_API JSEventHelper : public Object
{
    OBJECT(JSEventHelper);

public:
    /// Construct.
    JSEventHelper(Context* context);
    /// Destruct.
    virtual ~JSEventHelper();

    void AddEventHandler(StringHash eventType);
    void AddEventHandler(Object* sender, StringHash eventType);

private:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

};


}
