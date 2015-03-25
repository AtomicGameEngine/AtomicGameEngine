
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>

#include "NewProjectCmd.h"

#include <Poco/File.h>

namespace ToolCore
{

NewProjectCmd::NewProjectCmd(Context* context) : Command(context)
{

}

NewProjectCmd::~NewProjectCmd()
{

}

bool NewProjectCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "new")
    {
        errorMsg = "Unable to parse new command";
        return false;
    }

    if (!value.Length())
    {
        errorMsg = "Unable to parse new project path";
        return false;
    }

    projectPath_ = value;

    return true;
}

void NewProjectCmd::Run()
{
    Poco::File projectDest(projectPath_.CString());

    if (projectDest.exists())
    {
        Error(ToString("New project path: %s already exists", projectPath_.CString()));
        return;
    }

    LOGINFOF("Creating new project in: %s", projectPath_.CString());

    Finished();
}

}
