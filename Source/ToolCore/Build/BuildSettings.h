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

#pragma once

/*
#include <rapidjson/document.h>
#include <rapidjson/filestream.h>
#include <rapidjson/prettywriter.h>

#include <Atomic/Container/Str.h>
#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
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

struct IOSBuildSettings
{
    String appName;
    String package;
    String companyName;
    String productName;
    String provisionFile;
    String appidPrefix;
};

struct MacBuildSettings
{
    String appName;
    String package;
    String companyName;
    String productName;
};

struct WindowsBuildSettings
{
    String appName;
    String package;
    String companyName;
    String productName;
};

struct WebGLSettings
{
    String appName;
    String package;
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

    const IOSBuildSettings& GetIOSSettings() { return ios_; }
    void SetIOSSettings(const IOSBuildSettings& settings) { ios_ = settings; }

    const MacBuildSettings& GetMacSettings() { return mac_; }
    void SetMacSettings(const MacBuildSettings& settings) { mac_ = settings; }

    const WindowsBuildSettings& GetWindowsSettings() { return windows_; }
    void SetWindowsSettings(const WindowsBuildSettings& settings) { windows_ = settings; }

    const WebGLSettings& GetWebGLSettings() { return webgl_; }
    void SetWebGLSettings(const WebGLSettings& settings) { webgl_ = settings; }

    void Load(rapidjson::Value::Member* jobject);
    void Save(rapidjson::PrettyWriter<rapidjson::FileStream>& writer);

private:

    String GetStringMember(rapidjson::Value::Member* jobject, const String& name);

    AndroidBuildSettings android_;
    IOSBuildSettings ios_;
    MacBuildSettings mac_;
    WindowsBuildSettings windows_;
    WebGLSettings webgl_;

};

}

*/
