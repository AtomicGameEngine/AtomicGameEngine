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

