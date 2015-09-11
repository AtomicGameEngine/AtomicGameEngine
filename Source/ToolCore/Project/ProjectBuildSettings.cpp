//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include <Atomic/Resource/JSONFile.h>

#include "ProjectBuildSettings.h"

namespace ToolCore
{

void MacBuildSettings::Write(JSONValue& parent)
{
    JSONValue json;

    json.Set("appName", appName_);
    json.Set("packageName", packageName_);
    json.Set("companyName", companyName_);
    json.Set("productName", productName_);

    parent.Set("MacBuildSettings", json);
}

void MacBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.Get("MacBuildSettings");

    if (!json.IsObject())
        return;

    appName_ = json.Get("appName").GetString();
    packageName_ = json.Get("packageName").GetString();
    companyName_ = json.Get("companyName").GetString();
    productName_ = json.Get("productName").GetString();

}

void WindowsBuildSettings::Write(JSONValue& parent)
{
    JSONValue json;

    json.Set("appName", appName_);
    json.Set("packageName", packageName_);
    json.Set("companyName", companyName_);
    json.Set("productName", productName_);

    parent.Set("WindowsBuildSettings", json);

}

void WindowsBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.Get("WindowsBuildSettings");

    if (!json.IsObject())
        return;

    appName_ = json.Get("appName").GetString();
    packageName_ = json.Get("packageName").GetString();
    companyName_ = json.Get("companyName").GetString();
    productName_ = json.Get("productName").GetString();

}


void WebBuildSettings::Write(JSONValue& parent)
{
    JSONValue json;

    json.Set("appName", appName_);
    json.Set("packageName", packageName_);
    json.Set("companyName", companyName_);
    json.Set("productName", productName_);

    parent.Set("WebBuildSettings", json);

}

void WebBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.Get("WebBuildSettings");

    if (!json.IsObject())
        return;

    appName_ = json.Get("appName").GetString();
    packageName_ = json.Get("packageName").GetString();
    companyName_ = json.Get("companyName").GetString();
    productName_ = json.Get("productName").GetString();

}

void AndroidBuildSettings::Write(JSONValue& parent)
{
    JSONValue json;

    json.Set("appName", appName_);
    json.Set("packageName", packageName_);
    json.Set("companyName", companyName_);
    json.Set("productName", productName_);

    json.Set("targetSDKVersion", targetSDKVersion_);
    json.Set("minSDKVersion", minSDKVersion_);
    json.Set("activityName", activityName_);

    parent.Set("AndroidBuildSettings", json);


}

void AndroidBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.Get("AndroidBuildSettings");

    if (!json.IsObject())
        return;

    appName_ = json.Get("appName").GetString();
    packageName_ = json.Get("packageName").GetString();
    companyName_ = json.Get("companyName").GetString();
    productName_ = json.Get("productName").GetString();

    targetSDKVersion_ = json.Get("targetSDKVersion").GetString();
    minSDKVersion_ = json.Get("minSDKVersion").GetString();
    activityName_ = json.Get("activityName").GetString();

}


void IOSBuildSettings::Write(JSONValue& parent)
{
    JSONValue json;

    json.Set("appName", appName_);
    json.Set("packageName", packageName_);
    json.Set("companyName", companyName_);
    json.Set("productName", productName_);

    json.Set("provisionFile", provisionFile_);
    json.Set("appIDPrefix", appidPrefix_);

    parent.Set("IOSBuildSettings", json);

}

void IOSBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.Get("IOSBuildSettings");

    if (!json.IsObject())
        return;

    appName_ = json.Get("appName").GetString();
    packageName_ = json.Get("packageName").GetString();
    companyName_ = json.Get("companyName").GetString();
    productName_ = json.Get("productName").GetString();

    provisionFile_ = json.Get("provisionFile").GetString();;
    appidPrefix_ = json.Get("appIDPrefix").GetString();;

}

ProjectBuildSettings::ProjectBuildSettings(Context* context) : Object(context),
    macBuildSettings_(new MacBuildSettings()),
    windowsBuildSettings_(new WindowsBuildSettings()),
    webBuildSettings_(new WebBuildSettings()),
    androidBuildSettings_(new AndroidBuildSettings()),
    iosBuildSettings_(new IOSBuildSettings())
{

}

ProjectBuildSettings::~ProjectBuildSettings()
{

}

bool ProjectBuildSettings::Load(const String& path)
{
    SharedPtr<File> file(new File(context_, path));
    if (!file->IsOpen())
        return false;

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));
    bool result = jsonFile->Load(*file);
    file->Close();

    if (!result)
        return false;

    JSONValue root = jsonFile->GetRoot();
    if (!root.IsObject())
        return false;

    macBuildSettings_->Read(root);
    windowsBuildSettings_->Read(root);
    webBuildSettings_->Read(root);
    androidBuildSettings_->Read(root);
    iosBuildSettings_->Read(root);

    return true;
}

void ProjectBuildSettings::Save(const String& path)
{
    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue& root = jsonFile->GetRoot();

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    macBuildSettings_->Write(root);
    windowsBuildSettings_->Write(root);
    webBuildSettings_->Write(root);
    androidBuildSettings_->Write(root);
    iosBuildSettings_->Write(root);

    jsonFile->Save(*file, String("   "));

    file->Close();

}


}
