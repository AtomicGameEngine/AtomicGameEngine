//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

EVENT(E_COMMANDERROR, CommandError)
{
    PARAM(P_MESSAGE, Message);      // string
}


EVENT(E_COMMANDFINISHED, CommandFinished)
{

}

class Command : public Object
{
    OBJECT(Command);

public:

    Command(Context* context);
    virtual ~Command();

    bool Parse(const String& command);

    virtual bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg) = 0;

    virtual void Run() = 0;

    virtual void Finished();

    virtual void Error(const String& errorMsg);

    virtual void Cancel() {}

    virtual bool RequiresProjectLoad() { return true; }

    virtual bool RequiresLicenseValidation() { return false; }

private:

    float timeOut_;

};

}
