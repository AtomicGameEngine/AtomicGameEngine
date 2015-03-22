
#pragma once

#include <Atomic/Engine/Application.h>

using namespace Atomic;

namespace AtomicCLI
{

class CLI : public Application
{
    OBJECT(CLI);

public:

    CLI(Context* context);
    virtual ~CLI();

    /// Setup before engine initialization. Verify that a script file has been specified.
    virtual void Setup();
    /// Setup after engine initialization. Load the script and execute its start function.
    virtual void Start();
    /// Cleanup after the main loop. Run the script's stop function if it exists.
    virtual void Stop();

private:

};

}
