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

/*
#include "AtomicEditor.h"
#include <Atomic/Core/CoreEvents.h>
#include "AtomicEditor.h"
#include "BuildSettings.h"

namespace ToolCore
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

    // IOS ------
    rapidjson::Value::Member* jios = jobject->value.FindMember("ios");
    if (jios && jios->value.IsObject())
    {
        ios_.appName = GetStringMember(jios, "app_name");
        ios_.package = GetStringMember(jios, "package");
        ios_.provisionFile = GetStringMember(jios, "provision_file");
        ios_.companyName = GetStringMember(jios, "company_name");
        ios_.productName = GetStringMember(jios, "product_name");
        ios_.appidPrefix = GetStringMember(jios, "appid_prefix");
    }
    // END IOS ------

    // Mac ------
    rapidjson::Value::Member* jmac = jobject->value.FindMember("mac");
    if (jmac && jmac->value.IsObject())
    {
        mac_.appName = GetStringMember(jmac, "app_name");
        mac_.package = GetStringMember(jmac, "package");
        mac_.companyName = GetStringMember(jmac, "company_name");
        mac_.productName = GetStringMember(jmac, "product_name");
    }
    // END Mac ------

    // Windows ------
    rapidjson::Value::Member* jwindows = jobject->value.FindMember("windows");
    if (jwindows && jwindows->value.IsObject())
    {
        windows_.appName = GetStringMember(jwindows, "app_name");
        windows_.package = GetStringMember(jwindows, "package");
        windows_.companyName = GetStringMember(jwindows, "company_name");
        windows_.productName = GetStringMember(jwindows, "product_name");
    }
    // END Windows ------

    // WebGL ------
    rapidjson::Value::Member* jwebgl = jobject->value.FindMember("webgl");
    if (jwebgl && jwebgl->value.IsObject())
    {
        webgl_.appName = GetStringMember(jwebgl, "app_name");
        webgl_.package = GetStringMember(jwebgl, "package");
        webgl_.companyName = GetStringMember(jwebgl, "company_name");
        webgl_.productName = GetStringMember(jwebgl, "product_name");
    }
    // END WebGL ------

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

    // IOS ------
    writer.String("ios");
    writer.StartObject();

    writer.String("app_name");
    writer.String(ios_.appName.CString());
    writer.String("package");
    writer.String(ios_.package.CString());
    writer.String("provision_file");
    writer.String(ios_.provisionFile.CString());
    writer.String("company_name");
    writer.String(ios_.companyName.CString());
    writer.String("product_name");
    writer.String(ios_.productName.CString());
    writer.String("appid_prefix");
    writer.String(ios_.appidPrefix.CString());

    writer.EndObject();
    // END IOS ------

    // Mac ------
    writer.String("mac");
    writer.StartObject();

    writer.String("app_name");
    writer.String(mac_.appName.CString());
    writer.String("package");
    writer.String(mac_.package.CString());
    writer.String("company_name");
    writer.String(mac_.companyName.CString());
    writer.String("product_name");
    writer.String(mac_.productName.CString());

    writer.EndObject();
    // END Mac ------

    // Windows ------
    writer.String("windows");
    writer.StartObject();

    writer.String("app_name");
    writer.String(windows_.appName.CString());
    writer.String("package");
    writer.String(windows_.package.CString());
    writer.String("company_name");
    writer.String(windows_.companyName.CString());
    writer.String("product_name");
    writer.String(windows_.productName.CString());

    writer.EndObject();
    // END Windows ------

    // WebGL ------
    writer.String("webgl");
    writer.StartObject();

    writer.String("app_name");
    writer.String(webgl_.appName.CString());
    writer.String("package");
    writer.String(webgl_.package.CString());
    writer.String("company_name");
    writer.String(webgl_.companyName.CString());
    writer.String("product_name");
    writer.String(webgl_.productName.CString());

    writer.EndObject();
    // END WebGL ------


    writer.EndObject();

}

}
*/
