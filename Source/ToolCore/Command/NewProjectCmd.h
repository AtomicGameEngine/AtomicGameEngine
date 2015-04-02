
#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class NewProjectCmd: public Command
{
    OBJECT(NewProjectCmd);

public:

    NewProjectCmd(Context* context);
    virtual ~NewProjectCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

    void Run();

    bool RequiresProjectLoad() { return false; }

private:

    String projectPath_;

};

}
