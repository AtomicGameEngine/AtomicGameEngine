// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Core/CoreEvents.h>
#include "AtomicEditor.h"
#include "BuildSettings.h"

namespace AtomicEditor
{

BuildSettings::BuildSettings(Context* context) :
    Object(context)
{

}

BuildSettings::~BuildSettings()
{

}

String BuildSettings::GetStringMember(rapidjson::Value::Member* jobject, const String& name)
{
    rapidjson::Value::Member* member = jobject->value.FindMember(name.CString());

    if (!member || !member->value.IsString())
        return "";

    return member->value.GetString();
}

void BuildSettings::Load(rapidjson::Value::Member* jobject)
{
    // ANDROID ------
    rapidjson::Value::Member* jandroid = jobject->value.FindMember("android");
    if (jandroid && jandroid->value.IsObject())
    {
        android_.appName = GetStringMember(jandroid, "app_name");
        android_.package = GetStringMember(jandroid, "package");
        android_.targetSDKVersion = GetStringMember(jandroid, "target_sdk_version");
        android_.minSDKVersion = GetStringMember(jandroid, "min_sdk_version");
        android_.activityName = GetStringMember(jandroid, "activity_name");
        android_.companyName = GetStringMember(jandroid, "company_name");
        android_.productName = GetStringMember(jandroid, "product_name");
    }
    // END ANDROID ------
}

void BuildSettings::Save(rapidjson::PrettyWriter<rapidjson::FileStream>& writer)
{
    writer.String("build_settings");
    writer.StartObject();

    writer.String("version");
    writer.Int(1);

    // ANDROID ------
    writer.String("android");
    writer.StartObject();

    writer.String("app_name");
    writer.String(android_.appName.CString());
    writer.String("package");
    writer.String(android_.package.CString());
    writer.String("target_sdk_version");
    writer.String(android_.targetSDKVersion.CString());
    writer.String("min_sdk_version");
    writer.String(android_.minSDKVersion.CString());
    writer.String("activity_name");
    writer.String(android_.activityName.CString());
    writer.String("company_name");
    writer.String(android_.companyName.CString());
    writer.String("product_name");
    writer.String(android_.productName.CString());

    writer.EndObject();
    // END ANDROID ------

    writer.EndObject();

}

}

