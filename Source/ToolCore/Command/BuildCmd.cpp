
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"

#include "BuildCmd.h"

#include <Poco/File.h>

namespace ToolCore
{

BuildCmd::BuildCmd(Context* context) : Command(context)
{

}

BuildCmd::~BuildCmd()
{

}

bool BuildCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "build")
    {
        errorMsg = "Unable to parse build command";
        return false;
    }

    if (!value.Length())
    {
        errorMsg = "Unable to parse build platform";
        return false;
    }

    buildPlatform_ = value;

    return true;
}

void BuildCmd::Run()
{
    LOGINFOF("Building project for: %s", buildPlatform_.CString());

    Finished();
}

}
