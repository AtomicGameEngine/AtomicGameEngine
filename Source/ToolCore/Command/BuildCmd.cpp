//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "../Build/BuildEvents.h"
#include "../Build/BuildSystem.h"
#include "../Build/AssetBuildConfig.h"

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
    String tag = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;

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
    assetsBuildTag_ = tag.ToLower();

    return true;
}

void BuildCmd::HandleBuildComplete(StringHash eventType, VariantMap& eventData)
{
    if (!eventData[BuildComplete::P_SUCCESS].GetBool())
    {
        Error("Build Failed");
    }
    else
        Finished();

}



void BuildCmd::Run()
{
    ATOMIC_LOGINFOF("Building project for: %s", buildPlatform_.CString());

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    Platform* platform = NULL;

    platform = tsystem->GetPlatformByName(buildPlatform_);

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
    if (!assetsBuildTag_.Empty())
    {
        buildBase->SetAssetBuildTag(assetsBuildTag_);
    }

    // add it to the build system
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    buildSystem->QueueBuild(buildBase);

    SubscribeToEvent(E_BUILDCOMPLETE, ATOMIC_HANDLER(BuildCmd, HandleBuildComplete));

    // TODO: parallel/serial builds
    buildSystem->StartNextBuild();

}

}
