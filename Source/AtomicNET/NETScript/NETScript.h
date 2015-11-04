#pragma once

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Object.h>

namespace Atomic
{

class NETCore;
class CSManaged;

class ATOMIC_API NETScript : public Object
{

    OBJECT(NETScript);

public:
    /// Construct.
    NETScript(Context* context);
    /// Destruct.
    virtual ~NETScript();

    bool Initialize();

    bool ExecMainAssembly();

private:

    WeakPtr<NETCore> netCore_;
    WeakPtr<CSManaged> csManaged_;

};

void RegisterNETScriptLibrary(Context* context);

}
