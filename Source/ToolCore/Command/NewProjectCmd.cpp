
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"

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

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    String templateDir = tsystem->GetDataPath();
    templateDir += "/Atomic/ProjectTemplates/Project2D/Resources";

    Poco::File projectSrc(templateDir.CString());
    if (!projectSrc.exists() || !projectSrc.isDirectory())
    {
        Error(ToString("New project path: %s source does not exist", templateDir.CString()));
        return;
    }

    LOGINFOF("Creating new project in: %s", projectPath_.CString());

    projectDest.createDirectory();
    projectSrc.copyTo((projectPath_ + "/Resources").CString());

    String filename("NewProject");
    File file(context_, projectPath_ + "/" + filename + ".atomic", FILE_WRITE);
    file.Close();

    Finished();
}

}
