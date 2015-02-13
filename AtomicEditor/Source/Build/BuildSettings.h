// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include <rapidjson/document.h>
#include <rapidjson/filestream.h>
#include <rapidjson/prettywriter.h>

#include <Atomic/Container/Str.h>
#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

struct AndroidBuildSettings
{
    String appName;
    String package;
    String targetSDKVersion;
    String minSDKVersion;
    String activityName;
    String companyName;
    String productName;
};

class BuildSettings : public Object
{
    OBJECT(BuildSettings);

public:
    /// Construct.
    BuildSettings(Context* context);
    /// Destruct.
    virtual ~BuildSettings();

    const AndroidBuildSettings& GetAndroidSettings() { return android_; }
    void SetAndroidSettings(const AndroidBuildSettings& settings) { android_ = settings; }

    void Load(rapidjson::Value::Member* jobject);
    void Save(rapidjson::PrettyWriter<rapidjson::FileStream>& writer);

private:

    String GetStringMember(rapidjson::Value::Member* jobject, const String& name);

    AndroidBuildSettings android_;

};

}
