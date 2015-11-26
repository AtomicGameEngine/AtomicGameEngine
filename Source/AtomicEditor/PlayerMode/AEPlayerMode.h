//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCTypes.h>

#include <Atomic/UI/SystemUI/MessageBox.h>

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

    bool launchedByEditor() { return launchedByEditor_; }

private:

    void ProcessArguments();

    void HandleJSError(StringHash eventType, VariantMap& eventData);
    void HandleLogMessage(StringHash eventType, VariantMap& eventData);
    void HandleIPCInitialize(StringHash eventType, VariantMap& eventData);
    void HandleViewRender(StringHash eventType, VariantMap& eventData);
    void HandleExitRequest(StringHash eventType, VariantMap& eventData);

// BEGIN LICENSE MANAGEMENT
    void HandleMessageAck(StringHash eventType, VariantMap& eventData);
    bool licenseModule3D_;
    SharedPtr<SystemUI::MessageBox> messageBox_;
// END LICENSE MANAGEMENT

    IPCHandle fd_[2];
    WeakPtr<IPC> ipc_;
    bool brokerActive_;
    bool launchedByEditor_;

};

}
