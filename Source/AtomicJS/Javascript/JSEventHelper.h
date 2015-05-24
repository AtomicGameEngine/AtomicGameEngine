
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

    VariantMap& GetCurrentData() { return currentData_; }

private:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

    VariantMap& currentData_;

};


}
