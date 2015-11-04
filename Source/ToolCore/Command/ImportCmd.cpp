//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"

#include "../Import/JSONSceneImporter.h"
#include "../Import/JSONSceneProcess.h"
#include "../Import/OpenAssetImporter.h"

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

    assetFilename_ = value;

    return true;
}

void ImportCmd::Run()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();
    String resourcePath = project->GetResourcePath();

    String ext = GetExtension(assetFilename_);

    if (ext == ".json")
    {
        Poco::File file(assetFilename_.CString());

        if (!file.exists())
        {
            Error(ToString("JSON source scene does not exist: %s", assetFilename_.CString()));
            return;
        }

        LOGRAWF("Importing JSON: %s", assetFilename_.CString());

        SharedPtr<JSONSceneImporter> jimporter;
        jimporter = new JSONSceneImporter(context_);
        jimporter->Import(assetFilename_);

        SharedPtr<JSONSceneProcess> sceneProcess;
        sceneProcess = new JSONSceneProcess(context_, jimporter);
        sceneProcess->Process(resourcePath);
        sceneProcess->Write();
    }
    else
    {
        SharedPtr<OpenAssetImporter> importer(new OpenAssetImporter(context_));
        if (importer->Load(assetFilename_))
        {
            importer->ExportModel("/Users/josh/Desktop/ExportedModel.mdl");
        }

    }

    Finished();
}

}
