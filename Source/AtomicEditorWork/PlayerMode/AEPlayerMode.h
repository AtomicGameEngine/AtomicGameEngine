
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/IPC/IPCTypes.h>

using namespace Atomic;

namespace AtomicEditor
{

/// PlayerMode subsystem
class PlayerMode : public Object
{
    friend class AEPlayerApplication;

    OBJECT(PlayerMode);

public:
    /// Construct.
    PlayerMode(Context* context);
    /// Destruct.
    virtual ~PlayerMode();

private:

    void ProcessArguments();
    void HandleHelloFromBroker(StringHash eventType, VariantMap& eventData);

    IPCHandle fd_[2];

};

}
