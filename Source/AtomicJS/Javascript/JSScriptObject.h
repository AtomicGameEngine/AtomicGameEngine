
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

/// ScriptObject makes it possible for script classes to inherit "directly" from
/// Object (which is abstract) and use the event system, etc
class JSScriptObject : public Object
{

    OBJECT(JSScriptObject);

public:
    /// Construct.
    JSScriptObject(Context* context);
    /// Destruct.
    virtual ~JSScriptObject();

};

}
