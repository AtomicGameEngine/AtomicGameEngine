
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"

#include "PlatformAddCmd.h"

#include <Poco/File.h>

namespace ToolCore
{

PlatformAddCmd::PlatformAddCmd(Context* context) : Command(context)
{

}

PlatformAddCmd::~PlatformAddCmd()
{

}

bool PlatformAddCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "platform-add")
    {
        errorMsg = "Unable to parse build command";
        return false;
    }

    if (!value.Length())
    {
        errorMsg = "Unable to parse build platform";
        return false;
    }

    platformToAdd_ = value.ToLower();

    return true;
}

void PlatformAddCmd::Run()
{
    LOGINFOF("Adding platform: %s", platformToAdd_.CString());

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    Platform* platform = NULL;

    Finished();
}

}
