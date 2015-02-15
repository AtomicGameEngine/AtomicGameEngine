// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>

#include "BuildSystem.h"
#include "BuildMac.h"
#include "BuildWindows.h"
#include "BuildAndroid.h"
#include "BuildIOS.h"
#include "BuildWeb.h"

#include "../AEEvents.h"
#include "../AEEditor.h"

namespace AtomicEditor
{

BuildSystem::BuildSystem(Context* context) :
    Object(context)
{
    buildSettings_ = new BuildSettings(context);

    SubscribeToEvent(E_EDITORBUILD, HANDLER(BuildSystem, HandleEditorBuild));

}

BuildSystem::~BuildSystem()
{

}

void BuildSystem::BuildComplete()
{
    currentBuild_ = 0;
}

void BuildSystem::DoBuildMac(const String& buildPath)
{
    currentBuild_ = SharedPtr<BuildBase>(new BuildMac(context_));
    currentBuild_->Build(buildPath);
}

void BuildSystem::DoBuildWindows(const String& buildPath)
{
    currentBuild_ = SharedPtr<BuildBase>(new BuildWindows(context_));
    currentBuild_->Build(buildPath);
}

void BuildSystem::DoBuildAndroid(const String& buildPath)
{
    currentBuild_ = SharedPtr<BuildBase>(new BuildAndroid(context_));
    currentBuild_->Build(buildPath);

}

void BuildSystem::DoBuildWeb(const String& buildPath)
{
    currentBuild_ = SharedPtr<BuildBase>(new BuildWeb(context_));
    currentBuild_->Build(buildPath);

}

void BuildSystem::DoBuildIOS(const String& buildPath)
{
    currentBuild_ = SharedPtr<BuildBase>(new BuildIOS(context_));
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


void BuildSystem::HandleEditorBuild(StringHash eventType, VariantMap& eventData)
{
    using namespace EditorBuild;

    String buildPlatform = eventData[P_PLATFORM].GetString();
    String buildPath = eventData[P_BUILDPATH].GetString();

    if (buildPlatform == "Mac")
    {
        buildPath += "/AtomicPlayer.app";
    }

    Editor* editor = GetSubsystem<Editor>();
    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (buildPlatform == "Mac")
    {
        DoBuildMac(buildPath);
    }
    else if (buildPlatform == "Windows")
    {
        DoBuildWindows(buildPath);
    }
    else if (buildPlatform == "Android")
    {
        DoBuildAndroid(buildPath);
    }
    else if (buildPlatform == "HTML5")
    {
        DoBuildWeb(buildPath);
    }
    else if (buildPlatform == "IOS")
    {
        DoBuildIOS(buildPath);
    }

}

}
