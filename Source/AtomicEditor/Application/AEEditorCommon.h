
#pragma once

#include <Atomic/Engine/Application.h>

using namespace Atomic;

namespace Atomic
{
    class JSVM;
}

namespace AtomicEditor
{

class AEEditorCommon : public Application
{
    OBJECT(AEEditorCommon);

public:
    /// Construct.
    AEEditorCommon(Context* context);

    /// Setup before engine initialization. Verify that a script file has been specified.
    virtual void Setup();
    /// Setup after engine initialization. Load the script and execute its start function.
    virtual void Start();
    /// Cleanup after the main loop. Run the script's stop function if it exists.
    virtual void Stop();

protected:

    SharedPtr<JSVM> vm_;

private:


};

}
