#pragma once

#include <Atomic/IPC/IPC.h>
#include "AppBase.h"

namespace Atomic
{
    
    class IPCServerApp : public AppBase
    {

        ATOMIC_OBJECT(IPCServerApp, AppBase)

    public:
        /// Construct.
        IPCServerApp(Context* context);
        /// Destruct.
        virtual ~IPCServerApp();

        virtual void Setup();
        virtual void Stop();

        bool RunIPCPlayer(const String& projectName, const String& projectPath, const String &addArgs);

        void RequestTogglePlayerUpdatesPaused();
        void RequestPlayerPauseStep();
        void RequestPlayerExit();


    private:

        void HandleIPCWorkerStarted(StringHash eventType, VariantMap& eventData);
        void HandleIPCJSError(StringHash eventType, VariantMap& eventData);
        void HandleIPCWorkerLog(StringHash eventType, VariantMap& eventData);
        void HandleIPCWorkerExit(StringHash eventType, VariantMap& eventData);        
        
        SharedPtr<IPCBroker> playerBroker_;

        bool playerEnabled_;

    };

}
