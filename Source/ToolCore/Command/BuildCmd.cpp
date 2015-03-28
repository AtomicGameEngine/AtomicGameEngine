
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "../Build/BuildEvents.h"
#include "../Build/BuildSystem.h"

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

    buildPlatform_ = value.ToLower();

    return true;
}

void BuildCmd::HandleBuildComplete(StringHash eventType, VariantMap& eventData)
{
    Finished();
}

void BuildCmd::HandleBuildFailed(StringHash eventType, VariantMap& eventData)
{
    Error("Build Failed");
}

void BuildCmd::Run()
{
    LOGINFOF("Building project for: %s", buildPlatform_.CString());

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    Platform* platform = NULL;

    if (buildPlatform_ == "mac")
        platform = tsystem->GetPlatformByID(PLATFORMID_MAC);    
    else if (buildPlatform_ == "web")
        platform = tsystem->GetPlatformByID(PLATFORMID_WEB);

    if (!platform)
    {
        Error(ToString("Unknown build platform: %s", buildPlatform_.CString()));
        return;
    }

    if (!project->ContainsPlatform(platform->GetPlatformID()))
    {
        Error(ToString("Project does not contain platform: %s", buildPlatform_.CString()));
        return;
    }

    // create the build
    BuildBase* buildBase = platform->NewBuild(project);

    // add it to the build system
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    buildSystem->QueueBuild(buildBase);

    SubscribeToEvent(E_BUILDCOMPLETE, HANDLER(BuildCmd, HandleBuildComplete));
    SubscribeToEvent(E_BUILDFAILED, HANDLER(BuildCmd, HandleBuildFailed));

    // TODO: parallel/serial builds
    buildSystem->StartNextBuild();

}

}
