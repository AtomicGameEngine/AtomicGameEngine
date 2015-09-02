
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

ProjectBuildSettings::ProjectBuildSettings(Context* context) : Object(context),
    macBuildSettings_(new MacBuildSettings())
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

    return result;
}

void ProjectBuildSettings::Save(const String& path)
{
    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->CreateRoot();

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    macBuildSettings_->Write(root);

    jsonFile->Save(*file, String("   "));

    file->Close();

}


}
