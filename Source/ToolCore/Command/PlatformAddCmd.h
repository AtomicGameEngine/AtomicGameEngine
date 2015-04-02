
#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class PlatformAddCmd: public Command
{
    OBJECT(PlatformAddCmd);

public:

    PlatformAddCmd(Context* context);
    virtual ~PlatformAddCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

    void Run();

private:

    String platformToAdd_;

};

}
