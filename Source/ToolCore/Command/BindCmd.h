
#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class BindCmd: public Command
{
    OBJECT(BindCmd);

public:

    BindCmd(Context* context);
    virtual ~BindCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

    void Run();

    bool RequiresProjectLoad() { return false; }

private:

    String bindPlatform_;
    String sourceRootFolder_;
    String packageFolder_;

};

}
