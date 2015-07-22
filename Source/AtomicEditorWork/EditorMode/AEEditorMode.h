
#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{
    class IPCBroker;
}

using namespace Atomic;

namespace AtomicEditor
{

/// EditorMode subsystem
class EditorMode : public Object
{
    OBJECT(EditorMode);

public:
    /// Construct.
    EditorMode(Context* context);
    /// Destruct.
    virtual ~EditorMode();

    bool PlayProject();

private:

    void HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData);
    void HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData);

    SharedPtr<IPCBroker> playerBroker_;

};

}
