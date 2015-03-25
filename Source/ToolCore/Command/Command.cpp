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
    SendEvent(E_COMMANDFINISHED);
}

}

