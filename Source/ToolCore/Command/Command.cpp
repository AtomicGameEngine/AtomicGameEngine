//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
