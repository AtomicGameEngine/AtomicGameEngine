//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Resource/JSONValue.h>

using namespace Atomic;

namespace ToolCore
{

class MacBuildSettings : public RefCounted
{
    REFCOUNTED(MacBuildSettings)

public:

    MacBuildSettings() {}

    const String& GetAppName() const { return appName_; }
    const String& GetPackageName() const { return packageName_; }
    const String& GetCompanyName() const { return companyName_; }
    const String& GetProductName() const { return productName_; }

    void SetAppName(const String& name) { appName_ = name; }
    void SetPackageName(const String& packageName) { packageName_ = packageName; }
    void SetCompanyName(const String& companyName) { companyName_ = companyName; }
    void SetProductName(const String& productName) { productName_ = productName; }

    void Write(JSONValue& parent);
    void Read(JSONValue& parent);

private:

    String appName_;
    String packageName_;
    String companyName_;
    String productName_;
};

class WebBuildSettings : public RefCounted
{
    REFCOUNTED(WebBuildSettings)

public:

    WebBuildSettings() {}

    const String& GetAppName() const { return appName_; }
    const String& GetPackageName() const { return packageName_; }
    const String& GetCompanyName() const { return companyName_; }
    const String& GetProductName() const { return productName_; }

    void SetAppName(const String& name) { appName_ = name; }
    void SetPackageName(const String& packageName) { packageName_ = packageName; }
    void SetCompanyName(const String& companyName) { companyName_ = companyName; }
    void SetProductName(const String& productName) { productName_ = productName; }

    void Write(JSONValue& parent);
    void Read(JSONValue& parent);

private:

    String appName_;
    String packageName_;
    String companyName_;
    String productName_;
};

class WindowsBuildSettings : public RefCounted
{
    REFCOUNTED(WindowsBuildSettings)

public:

    WindowsBuildSettings() {}

    const String& GetAppName() const { return appName_; }
    const String& GetPackageName() const { return packageName_; }
    const String& GetCompanyName() const { return companyName_; }
    const String& GetProductName() const { return productName_; }

    void SetAppName(const String& name) { appName_ = name; }
    void SetPackageName(const String& packageName) { packageName_ = packageName; }
    void SetCompanyName(const String& companyName) { companyName_ = companyName; }
    void SetProductName(const String& productName) { productName_ = productName; }

    void Write(JSONValue& parent);
    void Read(JSONValue& parent);

private:

    String appName_;
    String packageName_;
    String companyName_;
    String productName_;
};

class AndroidBuildSettings : public RefCounted
{
    REFCOUNTED(AndroidBuildSettings)

public:

    AndroidBuildSettings() {}

    const String& GetAppName() const { return appName_; }
    const String& GetPackageName() const { return packageName_; }
    const String& GetCompanyName() const { return companyName_; }
    const String& GetProductName() const { return productName_; }

    const String& GetSDKVersion() const { return targetSDKVersion_; }
    const String& GetMinSDKVersion() const { return minSDKVersion_; }
    const String& GetActivityName() const { return activityName_; }

    void SetAppName(const String& name) { appName_ = name; }
    void SetPackageName(const String& packageName) { packageName_ = packageName; }
    void SetCompanyName(const String& companyName) { companyName_ = companyName; }
    void SetProductName(const String& productName) { productName_ = productName; }

    void SetSDKVersion(const String& value) { targetSDKVersion_ = value; }
    void SetMinSDKVersion(const String& value) { minSDKVersion_ = value; }
    void SetActivityName(const String& value) { activityName_ = value; }

    void Write(JSONValue& parent);
    void Read(JSONValue& parent);

private:

    String appName_;
    String packageName_;
    String companyName_;
    String productName_;

    String targetSDKVersion_;
    String minSDKVersion_;
    String activityName_;
};

class IOSBuildSettings : public RefCounted
{
    REFCOUNTED(IOSBuildSettings)

public:

    IOSBuildSettings() {}

    const String& GetAppName() const { return appName_; }
    const String& GetPackageName() const { return packageName_; }
    const String& GetCompanyName() const { return companyName_; }
    const String& GetProductName() const { return productName_; }

    const String& GetProvisionFile() const { return provisionFile_; }
    const String& GetAppIDPrefix() const { return appidPrefix_; }

    void SetAppName(const String& name) { appName_ = name; }
    void SetPackageName(const String& packageName) { packageName_ = packageName; }
    void SetCompanyName(const String& companyName) { companyName_ = companyName; }
    void SetProductName(const String& productName) { productName_ = productName; }

    void SetProvisionFile(const String& value) { provisionFile_ = value; }
    void SetAppIDPrefix(const String& value) { appidPrefix_ = value; }

    void Write(JSONValue& parent);
    void Read(JSONValue& parent);

private:

    String appName_;
    String packageName_;
    String companyName_;
    String productName_;

    String provisionFile_;
    String appidPrefix_;
};

class ProjectBuildSettings : public Object
{
    OBJECT(ProjectBuildSettings);

public:
    /// Construct.
    ProjectBuildSettings(Context* context);
    /// Destruct.
    virtual ~ProjectBuildSettings();

    MacBuildSettings* GetMacBuildSettings() { return macBuildSettings_; }
    WindowsBuildSettings* GetWindowsBuildSettings() { return windowsBuildSettings_; }
    WebBuildSettings* GetWebBuildSettings() { return webBuildSettings_; }
    AndroidBuildSettings* GetAndroidBuildSettings() { return androidBuildSettings_; }
    IOSBuildSettings* GetIOSBuildSettings() { return iosBuildSettings_; }

    bool Load(const String& path);
    void Save(const String& path);

private:

    SharedPtr<MacBuildSettings> macBuildSettings_;
    SharedPtr<WindowsBuildSettings> windowsBuildSettings_;
    SharedPtr<WebBuildSettings> webBuildSettings_;
    SharedPtr<AndroidBuildSettings> androidBuildSettings_;
    SharedPtr<IOSBuildSettings> iosBuildSettings_;

};

}
