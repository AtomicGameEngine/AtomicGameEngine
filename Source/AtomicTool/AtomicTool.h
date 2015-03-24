
#pragma once

#include <Atomic/Engine/Application.h>

using namespace Atomic;

namespace AtomicTool
{

class AtomicTool : public Application
{
    OBJECT(AtomicTool);

public:

    AtomicTool(Context* context);
    virtual ~AtomicTool();

    /// Setup before engine initialization. Verify that a script file has been specified.
    virtual void Setup();
    /// Setup after engine initialization. Load the script and execute its start function.
    virtual void Start();
    /// Cleanup after the main loop. Run the script's stop function if it exists.
    virtual void Stop();

private:

};

}
