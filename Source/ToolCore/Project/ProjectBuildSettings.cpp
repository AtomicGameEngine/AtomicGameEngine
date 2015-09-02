
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include <Atomic/Resource/JSONFile.h>

#include "ProjectBuildSettings.h"

namespace ToolCore
{

void MacBuildSettings::Write(JSONValue& parent)
{
    JSONValue json = parent.CreateChild("MacBuildSettings");

    json.SetString("appName", appName_);
    json.SetString("packageName", packageName_);
    json.SetString("companyName", companyName_);
    json.SetString("productName", productName_);

}

void MacBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.GetChild("MacBuildSettings");

    if (json == JSONValue::EMPTY)
        return;

    appName_ = json.GetString("appName");
    packageName_ = json.GetString("packageName");
    companyName_ = json.GetString("companyName");
    productName_ = json.GetString("productName");

}

void WindowsBuildSettings::Write(JSONValue& parent)
{
    JSONValue json = parent.CreateChild("WindowsBuildSettings");

    json.SetString("appName", appName_);
    json.SetString("packageName", packageName_);
    json.SetString("companyName", companyName_);
    json.SetString("productName", productName_);

}

void WindowsBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.GetChild("WindowsBuildSettings");

    if (json == JSONValue::EMPTY)
        return;

    appName_ = json.GetString("appName");
    packageName_ = json.GetString("packageName");
    companyName_ = json.GetString("companyName");
    productName_ = json.GetString("productName");

}


void WebBuildSettings::Write(JSONValue& parent)
{
    JSONValue json = parent.CreateChild("WebBuildSettings");

    json.SetString("appName", appName_);
    json.SetString("packageName", packageName_);
    json.SetString("companyName", companyName_);
    json.SetString("productName", productName_);

}

void WebBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.GetChild("WebBuildSettings");

    if (json == JSONValue::EMPTY)
        return;

    appName_ = json.GetString("appName");
    packageName_ = json.GetString("packageName");
    companyName_ = json.GetString("companyName");
    productName_ = json.GetString("productName");

}

void AndroidBuildSettings::Write(JSONValue& parent)
{
    JSONValue json = parent.CreateChild("AndroidBuildSettings");

    json.SetString("appName", appName_);
    json.SetString("packageName", packageName_);
    json.SetString("companyName", companyName_);
    json.SetString("productName", productName_);

    json.SetString("targetSDKVersion", targetSDKVersion_);
    json.SetString("minSDKVersion", minSDKVersion_);
    json.SetString("activityName", activityName_);


}

void AndroidBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.GetChild("AndroidBuildSettings");

    if (json == JSONValue::EMPTY)
        return;

    appName_ = json.GetString("appName");
    packageName_ = json.GetString("packageName");
    companyName_ = json.GetString("companyName");
    productName_ = json.GetString("productName");

    targetSDKVersion_ = json.GetString("targetSDKVersion");
    minSDKVersion_ = json.GetString("minSDKVersion");
    activityName_ = json.GetString("activityName");

}


void IOSBuildSettings::Write(JSONValue& parent)
{
    JSONValue json = parent.CreateChild("IOSBuildSettings");

    json.SetString("appName", appName_);
    json.SetString("packageName", packageName_);
    json.SetString("companyName", companyName_);
    json.SetString("productName", productName_);

    json.SetString("provisionFile", provisionFile_);
    json.SetString("appIDPrefix", appidPrefix_);


}

void IOSBuildSettings::Read(JSONValue& parent)
{
    JSONValue json = parent.GetChild("IOSBuildSettings");

    if (json == JSONValue::EMPTY)
        return;

    appName_ = json.GetString("appName");
    packageName_ = json.GetString("packageName");
    companyName_ = json.GetString("companyName");
    productName_ = json.GetString("productName");

    provisionFile_ = json.GetString("provisionFile");
    appidPrefix_ = json.GetString("appIDPrefix");

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

    JSONValue root = jsonFile->GetRoot();
    if (root == JSONValue::EMPTY)
        return false;

    macBuildSettings_->Read(root);
    windowsBuildSettings_->Read(root);
    webBuildSettings_->Read(root);
    androidBuildSettings_->Read(root);
    iosBuildSettings_->Read(root);

    return result;
}

void ProjectBuildSettings::Save(const String& path)
{
    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->CreateRoot();

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
