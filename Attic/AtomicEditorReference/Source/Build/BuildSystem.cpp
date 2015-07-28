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

#include "UI/Modal/UIModalOps.h"

#include "../AEEvents.h"
#include "../AEEditor.h"

namespace AtomicEditor
{

BuildSystem::BuildSystem(Context* context) :
    Object(context)
{
    buildSettings_ = new BuildSettings(context);

    SubscribeToEvent(E_EDITORBUILD, HANDLER(BuildSystem, HandleEditorBuild));
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(BuildSystem, HandleEditorShutdown));

}

BuildSystem::~BuildSystem()
{

}

void BuildSystem::ClearBuildCompleteUI()
{
    if (uiBuildComplete_.Null())
        return;

    uiBuildComplete_->Hide();

    uiBuildComplete_ = 0;

}

void BuildSystem::BuildComplete(AEEditorPlatform platform, const String &buildFolder, bool success, bool fail3D)
{
    String title;
    String message;

    if (platform == AE_PLATFORM_ANDROID)
    {
        title = "Android Deployment";
        message = "Android Deployment";
    }
    else if (platform == AE_PLATFORM_IOS)
    {
        title = "iOS Deployment";
        message = "iOS Deployment";
    }
    else if (platform == AE_PLATFORM_MAC)
    {
        title = "Mac Deployment";
        message = "Mac Deployment";
    }
    else if (platform == AE_PLATFORM_WINDOWS)
    {
        title = "Windows Deployment";
        message = "Windows Deployment";
    }
    else if (platform == AE_PLATFORM_HTML5)
    {
        title = "Web Deployment";
        message = "Web Deployment";
    }

    if (success)
    {
        title += " Success";
        message += " Success";
    }
    else
    {
        title += " Failed";
        message += " Failed";
    }

// BEGIN LICENSE MANAGEMENT
    if (fail3D)
    {
        UIModalOps* ops = GetSubsystem<UIModalOps>();
        ops->ShowInfoModule3D();
    }
    else
    {
        uiBuildComplete_ = new UIBuildComplete(context_, title, message, buildFolder, success);
        uiBuildComplete_->Show();
    }

// END LICENSE MANAGEMENT

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

void BuildSystem::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}
