
#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class PlayCmd: public Command
{
    OBJECT(PlayCmd);

public:

    PlayCmd(Context* context);
    virtual ~PlayCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

    void Run();

private:

    bool LaunchPlayerProcess(const String& command, const Vector<String>& args, const String& initialDirectory);

};

}
