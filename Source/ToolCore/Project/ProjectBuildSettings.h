
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


class ProjectBuildSettings : public Object
{
    OBJECT(ProjectBuildSettings);

public:
    /// Construct.
    ProjectBuildSettings(Context* context);
    /// Destruct.
    virtual ~ProjectBuildSettings();

    MacBuildSettings* GetMacBuildSettings() { return macBuildSettings_; }

    bool Load(const String& path);
    void Save(const String& path);

private:

    SharedPtr<MacBuildSettings> macBuildSettings_;

};

}
