//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "CommandParser.h"

#include "NewProjectCmd.h"
#include "PlatformAddCmd.h"
#include "BuildCmd.h"
#include "ImportCmd.h"
#include "PlayCmd.h"
#include "EditCmd.h"
#include "BindCmd.h"
#include "NETCmd.h"

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
            else if (argument == "net")
            {
                cmd = new NETCmd(context_);
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
