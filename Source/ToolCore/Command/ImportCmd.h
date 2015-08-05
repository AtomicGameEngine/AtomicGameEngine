
#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class ImportCmd: public Command
{
    OBJECT(ImportCmd);

public:

    ImportCmd(Context* context);
    virtual ~ImportCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);
    void Run();

    bool RequiresProjectLoad() { return true; }

private:

    String assetFilename_;

};

}
