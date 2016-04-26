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

#include "../ToolSystem.h"
#include "../Project/Project.h"

#include "Command.h"

namespace ToolCore
{

Command::Command(Context* context) : Object(context),
    timeOut_(0.0f)
{

}

Command::~Command()
{

}

bool Command::Parse(const String& command)
{
    Vector<String> args = command.Split(' ');
    String errorMsg;

    return Parse(args, 0, errorMsg);
}

void Command::Error(const String& errorMsg)
{
    VariantMap eventData;
    eventData[CommandError::P_MESSAGE] = errorMsg;
    SendEvent(E_COMMANDERROR, eventData);
}

void Command::Finished()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    if (project && project->IsDirty())
        project->Save(project->GetProjectFilePath());

    SendEvent(E_COMMANDFINISHED);
}

}
