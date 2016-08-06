//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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

    ATOMIC_OBJECT(PlayerMode, Object);

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
    void HandlePlayerWindowChanged(StringHash eventType, VariantMap& eventData);
    void HandleUpdatesPausedResumed(StringHash eventType, VariantMap& eventData);

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
