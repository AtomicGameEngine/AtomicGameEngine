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
