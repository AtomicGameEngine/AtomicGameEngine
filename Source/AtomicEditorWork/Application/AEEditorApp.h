
#pragma once

#include <Atomic/Engine/Application.h>

using namespace Atomic;

namespace Atomic
{
    class JSVM;
}

namespace AtomicEditor
{

class AEPreferences;

class AEEditorApp : public Application
{
    OBJECT(AEEditorApp);

public:
    /// Construct.
    AEEditorApp(Context* context);

    /// Setup before engine initialization. Verify that a script file has been specified.
    virtual void Setup();
    /// Setup after engine initialization. Load the script and execute its start function.
    virtual void Start();
    /// Cleanup after the main loop. Run the script's stop function if it exists.
    virtual void Stop();

private:

    void HandleExitRequested(StringHash eventType, VariantMap& eventData);
    void HandleJSError(StringHash eventType, VariantMap& eventData);

    SharedPtr<JSVM> vm_;


};

}
