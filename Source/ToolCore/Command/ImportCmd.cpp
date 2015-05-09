
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"

#include "../Import/JSONSceneImporter.h"
#include "../Import/JSONSceneProcess.h"

#include "ImportCmd.h"

#include <Poco/File.h>

namespace ToolCore
{

ImportCmd::ImportCmd(Context* context) : Command(context)
{

}

ImportCmd::~ImportCmd()
{

}

bool ImportCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "import")
    {
        errorMsg = "Unable to parse import command";
        return false;
    }

    if (!value.Length())
    {
        errorMsg = "Unable to parse source JSON filename";
        return false;
    }

    sourceJSONFilename_ = value;

    return true;
}

void ImportCmd::Run()
{

    Poco::File file(sourceJSONFilename_.CString());

    if (!file.exists())
    {
        Error(ToString("JSON source scene does not exist: %s", sourceJSONFilename_.CString()));
        return;
    }

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    String resourcePath = project->GetResourcePath();

    LOGRAWF("Importing: %s", sourceJSONFilename_.CString());

    SharedPtr<JSONSceneImporter> jimporter;
    jimporter = new JSONSceneImporter(context_);
    jimporter->Import(sourceJSONFilename_);

    SharedPtr<JSONSceneProcess> sceneProcess;
    sceneProcess = new JSONSceneProcess(context_, jimporter);
    sceneProcess->Process(resourcePath);
    sceneProcess->Write();

    Finished();
}

}
