
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

/// ScriptObject makes it possible for script classes to inherit "directly" from
/// Object (which is abstract) and use the event system, etc
class ScriptObject : public Object
{

    OBJECT(ScriptObject);

public:
    /// Construct.
    ScriptObject(Context* context);
    /// Destruct.
    virtual ~ScriptObject();

};

}
