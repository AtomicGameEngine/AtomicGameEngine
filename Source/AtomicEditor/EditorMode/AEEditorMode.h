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
    ATOMIC_OBJECT(EditorMode, Object)

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
    void HandleIPCPlayerPauseResumeRequest(StringHash eventType, VariantMap& eventData);
    void HandleIPCPlayerUpdatesPausedResumed(StringHash eventType, VariantMap& eventData);
    void HandleIPCPlayerPauseStepRequest(StringHash eventType, VariantMap& eventData);
    void HandleIPCPlayerExitRequest(StringHash eventType, VariantMap& eventData);

    SharedPtr<IPCBroker> playerBroker_;

    bool playerEnabled_ = false;

};

}
