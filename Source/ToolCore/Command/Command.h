
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

private:

    float timeOut_;

};

}
