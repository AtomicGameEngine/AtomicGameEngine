// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

// BEGIN LICENSE MANAGEMENT

#pragma once

#include <Atomic/Core/Object.h>
#include "../AETypes.h"

#include "Net/CurlManager.h"

using namespace Atomic;

namespace AtomicEditor
{

class LicenseSystem : public Object
{
    OBJECT(LicenseSystem);

public:

    struct LicenseParse
    {
        bool licenseWindows_;
        bool licenseMac_;
        bool licenseAndroid_;
        bool licenseIOS_;
        bool licenseHTML5_;
        bool licenseModule3D_;

        LicenseParse()
        {
            licenseWindows_ = false;
            licenseMac_ = false;
            licenseAndroid_ = false;
            licenseIOS_ = false;
            licenseHTML5_ = false;
            licenseModule3D_ = false;
        }
    };


    /// Construct.
    LicenseSystem(Context* context);
    /// Destruct.
    virtual ~LicenseSystem();

    void Initialize();

    bool LicenseWindows() { return licenseWindows_; }
    bool LicenseMac() { return licenseMac_; }
    bool LicenseAndroid() { return licenseAndroid_; }
    bool LicenseIOS() { return licenseIOS_; }
    bool LicenseHTML5() { return licenseHTML5_; }
    bool LicenseModule3D() { return licenseModule3D_; }

    /// Returns whether there are any platform licenses available
    bool IsStandardLicense();

    bool RequestPlatformChange(AEEditorPlatform platform);

    void Activate(const String& key, const LicenseParse& parse);
    SharedPtr<CurlRequest>& Deactivate();

    void ResetLicense();

    /// Basic key validation
    bool ValidateKey(const String &key);

    const String& GetKey() { return key_; }
    String GenerateMachineID();
    String GetEmail() { return email_;}

    void LicenseAgreementConfirmed();

    int ParseResponse(const String& response, LicenseParse &parse);

private:

    void RequestServerVerification(const String& key);

    bool LoadLicense();
    void SaveLicense();
    void RemoveLicense();

    void HandleVerification(StringHash eventType, VariantMap& eventData);
    void HandleDeactivate(StringHash eventType, VariantMap& eventData);
    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    bool eulaAgreementConfirmed_;

    String key_;
    String email_;
    bool licenseWindows_;
    bool licenseMac_;
    bool licenseAndroid_;
    bool licenseIOS_;
    bool licenseHTML5_;
    bool licenseModule3D_;

    SharedPtr<CurlRequest> serverVerification_;
    SharedPtr<CurlRequest> deactivate_;


};

}

// END LICENSE MANAGEMENT
