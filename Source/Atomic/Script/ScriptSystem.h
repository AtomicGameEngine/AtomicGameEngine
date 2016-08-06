
#include "../Core/Context.h"
#include "../Core/Object.h"

#pragma once

namespace Atomic
{

class ScriptSystem : public Object
{
    ATOMIC_OBJECT(ScriptSystem, Object);

public:

    /// Construct.
    ScriptSystem(Context* context);
    /// Destruct.
    virtual ~ScriptSystem();

    static Context* GetContext() { return scriptContext_; }

private:

    static WeakPtr<Context> scriptContext_;

};

}
