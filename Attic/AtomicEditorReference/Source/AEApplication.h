// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Engine/Application.h>

using namespace Atomic;

namespace AtomicEditor
{

/// AtomicEditor application runs a script specified on the command line.
class AEApplication : public Application
{
    OBJECT(AEApplication);

public:
    /// Construct.
    AEApplication(Context* context);

    /// Setup before engine initialization. Verify that a script file has been specified.
    virtual void Setup();
    /// Setup after engine initialization. Load the script and execute its start function.
    virtual void Start();
    /// Cleanup after the main loop. Run the script's stop function if it exists.
    virtual void Stop();

private:

    String cmdLineProjectFile_;
    
};

}
