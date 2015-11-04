//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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
