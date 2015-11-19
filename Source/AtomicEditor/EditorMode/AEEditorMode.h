//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

#include "AEEditorEvents.h"

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

    bool PlayProject(String addArgs = "", bool debug = false);
    bool IsPlayerEnabled();

private:

    void HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData);
    void HandleIPCJSError(StringHash eventType, VariantMap& eventData);
    void HandleIPCWorkerLog(StringHash eventType, VariantMap& eventData);
    void HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData);
    void HandleIPCPlayerExitRequest(StringHash eventType, VariantMap& eventData);

    SharedPtr<IPCBroker> playerBroker_;

    bool playerEnabled_ = false;

};

}
