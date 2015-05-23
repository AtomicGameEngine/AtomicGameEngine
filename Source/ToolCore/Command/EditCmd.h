
#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class EditCmd: public Command
{
    OBJECT(EditCmd);

public:

    EditCmd(Context* context);
    virtual ~EditCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

    void Run();

private:

    bool LaunchEditorProcess(const String& command, const Vector<String>& args, const String& initialDirectory);

};

}
