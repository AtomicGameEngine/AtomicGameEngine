
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class Command;

class CommandParser : public Object
{
    OBJECT(CommandParser);

public:

    CommandParser(Context* context);
    virtual ~CommandParser();

    Command* Parse(const Vector<String>& arguments);

    const String& GetErrorMessage() const { return errorMsg_; }

private:

    String errorMsg_;

};

}
