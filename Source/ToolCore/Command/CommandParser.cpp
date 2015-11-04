//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include "CommandParser.h"

#include "NewProjectCmd.h"
#include "PlatformAddCmd.h"
#include "BuildCmd.h"
#include "ImportCmd.h"
#include "PlayCmd.h"
#include "EditCmd.h"
#include "BindCmd.h"
#include "NETProjectGenCmd.h"

namespace ToolCore
{

CommandParser::CommandParser(Context* context) : Object(context)
{

}

CommandParser::~CommandParser()
{

}

Command* CommandParser::Parse(const Vector<String>& arguments)
{
    Command* cmd = NULL;

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length())
        {
            String argument = arguments[i].ToLower();

            if (argument == "new")
            {
                cmd = new NewProjectCmd(context_);
            }
            else if (argument == "build")
            {
                cmd = new BuildCmd(context_);
            }
            else if (argument == "platform-add")
            {
                cmd = new PlatformAddCmd(context_);
            }
            else if (argument == "import")
            {
                cmd = new ImportCmd(context_);
            }
            else if (argument == "play")
            {
                cmd = new PlayCmd(context_);
            }
            else if (argument == "edit")
            {
                cmd = new EditCmd(context_);
            }
            else if (argument == "bind")
            {
                cmd = new BindCmd(context_);
            }
            else if (argument == "net-projectgen")
            {
                cmd = new NETProjectGenCmd(context_);
            }

        }

        if (cmd)
        {
            if (cmd->Parse(arguments, i, errorMsg_))
                return cmd;

            break;
        }

    }

    return NULL;
}

}
