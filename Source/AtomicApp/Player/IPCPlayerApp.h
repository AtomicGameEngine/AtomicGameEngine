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

#include "PlayerApp.h"
#include <Atomic/IPC/IPC.h>

namespace Atomic
{
    class IPC;

    class IPCPlayerApp : public PlayerApp
    {
        ATOMIC_OBJECT(IPCPlayerApp, PlayerApp)

    public:
        /// Construct.
        IPCPlayerApp(Context* context);
        virtual ~IPCPlayerApp();

        /// Setup before engine initialization. 
        virtual void Setup();
        /// Setup after engine initialization. L
        virtual void Start();
        /// Cleanup after the main loop. 
        virtual void Stop();

        virtual void ProcessArguments();

    protected:

    private:
        
        void HandleIPCInitialize(StringHash eventType, VariantMap& eventData);
        void HandleLogMessage(StringHash eventType, VariantMap& eventData);
        void HandleJSError(StringHash eventType, VariantMap& eventData);

        void HandlePlayerWindowChanged(StringHash eventType, VariantMap& eventData);
        void HandleUpdatesPausedResumed(StringHash eventType, VariantMap& eventData);
        void HandleQuit(StringHash eventType, VariantMap& eventData);
        void HandleExitRequest(StringHash eventType, VariantMap& eventData);

        /// true if we're launched as a subprocess, otherwise top level process (generally for debugging)
        bool subprocess_;
        bool debugPlayer_;

        IPCHandle fd_[2];
        WeakPtr<IPC> ipc_;
        bool brokerActive_;
    };

}
