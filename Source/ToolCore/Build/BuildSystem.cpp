// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>

#include "BuildSystem.h"
#include "BuildWeb.h"


namespace ToolCore
{

BuildSystem::BuildSystem(Context* context) :
    Object(context)
{
    buildSettings_ = new BuildSettings(context);
}

BuildSystem::~BuildSystem()
{

}

void BuildSystem::BuildComplete(PlatformID platform, const String &buildFolder, bool success, bool fail3D)
{

    if (success)
        LOGINFOF("Build Success");
    else
        LOGINFOF("Build Failed");

    currentBuild_ = 0;
}


void BuildSystem::DoBuildWeb(const String& buildPath)
{
    currentBuild_ = SharedPtr<BuildBase>(new BuildWeb(context_));
    currentBuild_->Build(buildPath);

}

void BuildSystem::LoadBuildSettings(rapidjson::Value::Member* jobject)
{
    buildSettings_->Load(jobject);
}

void BuildSystem::SaveBuildSettings(rapidjson::PrettyWriter<rapidjson::FileStream>& writer)
{
    buildSettings_->Save(writer);
}




}
