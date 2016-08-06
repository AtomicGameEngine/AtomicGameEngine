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

// BEGIN LICENSE MANAGEMENT

#ifdef ATOMIC_PLATFORM_WINDOWS
#ifndef _MSC_VER
#define _WIN32_IE 0x501
#endif
#include <windows.h>
#include <shellapi.h>
#include <direct.h>
#include <shlobj.h>
#include <sys/types.h>
#include <sys/utime.h>
#endif

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Core/Context.h>
#include <Atomic/Core/Timer.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/Log.h>

#include "LicenseEvents.h"
#include "LicenseSystem.h"

#include <Poco/MD5Engine.h>
#include <Poco/File.h>

namespace ToolCore
{

LicenseSystem::LicenseSystem(Context* context) :
    Object(context)
    , eulaAgreementConfirmed_(false)

{
    FileSystem* filesystem = GetSubsystem<FileSystem>();

    licenseFilePath_ = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    licenseFilePath_ = AddTrailingSlash(licenseFilePath_);

    if (!filesystem->DirExists(licenseFilePath_))
    {
        Poco::File dirs(licenseFilePath_.CString());
        dirs.createDirectories();
    }

    licenseCachePath_ = licenseFilePath_;

    licenseCachePath_ += "AtomicLicenseCache";

    licenseFilePath_ += "AtomicLicense";

    eulaAgreementPath_ = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    eulaAgreementPath_ = AddTrailingSlash(eulaAgreementPath_);
    eulaAgreementPath_ += "EulaConfirmed";

    ResetLicense();
}

LicenseSystem::~LicenseSystem()
{

}

void LicenseSystem::Initialize()
{

    FileSystem* filesystem = GetSubsystem<FileSystem>();

    eulaAgreementConfirmed_ = filesystem->FileExists(eulaAgreementPath_);

    if (!eulaAgreementConfirmed_)
    {
        SendEvent(E_LICENSE_EULAREQUIRED);
        return;
    }
    else
    {
        SendEvent(E_LICENSE_EULAACCEPTED);
    }

    // TODO: Cleanup for MIT

    if (!LoadLicense() || !key_.Length())
    {
        ResetLicense();

        SendEvent(E_LICENSE_ACTIVATIONREQUIRED);
        return;
    }
    else
    {
        // RequestServerVerification(key_);
    }
}

void LicenseSystem::LicenseAgreementConfirmed()
{
    eulaAgreementConfirmed_ = true;

    SharedPtr<File> file(new File(context_, eulaAgreementPath_, FILE_WRITE));
    file->WriteInt(1);
    file->Close();

    if (!LoadLicense() || !key_.Length())
        SendEvent(E_LICENSE_ACTIVATIONREQUIRED);
}

String LicenseSystem::GenerateMachineID()
{
#if defined(ATOMIC_PLATFORM_OSX) || defined(ATOMIC_PLATFORM_LINUX)
    String path = getenv("HOME");
#else
    wchar_t pathName[MAX_PATH];
    pathName[0] = 0;
    SHGetSpecialFolderPathW(0, pathName, CSIDL_PERSONAL, 0);
    String path(pathName);
#endif

    Poco::MD5Engine md5;
    md5.update(path.CString(), path.Length());
    String id = Poco::MD5Engine::digestToHex(md5.digest()).c_str();
    return id;
}

void LicenseSystem::ResetLicense()
{
    key_ = "";
    licenseWindows_ = false;
    licenseMac_ = false;
    licenseAndroid_ = false;
    licenseIOS_ = false;
    licenseHTML5_ = false;
    licenseModule3D_ = false;

}

bool LicenseSystem::LoadLicense()
{

    key_ = "ATOMIC-XXXX-XXXX-XXXX-XXXX";
    licenseWindows_ = true;
    licenseMac_ = true;
    licenseAndroid_ = true;
    licenseIOS_ = true;
    licenseHTML5_ = true;
    licenseModule3D_ = true;

    return true;

   // TODO: Cleanup for MIT
/*
    ResetLicense();

    FileSystem* filesystem = GetSubsystem<FileSystem>();

    if (!filesystem->FileExists(licenseFilePath_))
        return false;

    SharedPtr<File> file(new File(context_, licenseFilePath_, FILE_READ));

    file->ReadInt(); // version

    String key = file->ReadString();
    if (!ValidateKey(key))
        return false;

    key_ = key;

    licenseWindows_ = file->ReadBool();
    licenseMac_ = file->ReadBool();
    licenseAndroid_ = file->ReadBool();
    licenseIOS_ = file->ReadBool();
    licenseHTML5_ = file->ReadBool();
    licenseModule3D_ = file->ReadBool();

    return true;
*/
}

bool LicenseSystem::ValidateKey(const String& key)
{
    if (!key.StartsWith("ATOMIC-"))
        return false;

    Vector<String> elements = key.Split('-');
    if (elements.Size() != 5)
        return false;

    for (unsigned i = 1; i < elements.Size(); i++)
    {
        String element = elements[i];
        if (element.Length() != 4)
            return false;

        for (unsigned j = 0; j < 4; j++)
        {
            char c = element[j];
            if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
                continue;
            return false;
        }
    }

    return true;
}


void LicenseSystem::SaveLicense()
{
    SharedPtr<File> file(new File(context_, licenseFilePath_, FILE_WRITE));
    file->WriteInt(1); // version
    file->WriteString(key_);

    file->WriteBool(licenseWindows_);
    file->WriteBool(licenseMac_);
    file->WriteBool(licenseAndroid_);
    file->WriteBool(licenseIOS_);
    file->WriteBool(licenseHTML5_);
    file->WriteBool(licenseModule3D_);

    file->Close();

}

void LicenseSystem::RemoveLicense()
{
    FileSystem* filesystem = GetSubsystem<FileSystem>();

    if (filesystem->FileExists(licenseFilePath_))
    {
        filesystem->Delete(licenseFilePath_);
    }

    if (filesystem->FileExists(licenseCachePath_))
    {
        filesystem->Delete(licenseCachePath_);
    }
}

bool LicenseSystem::IsStandardLicense()
{
    return !licenseAndroid_;
}


void LicenseSystem::RequestServerVerification(const String& key)
{
    if (serverVerification_.NotNull())
    {
        ATOMIC_LOGERROR("LicenseSystem::RequestServerLicense - request already exists");
        return;
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (fileSystem->FileExists(licenseCachePath_))
    {
        Time* time = GetSubsystem<Time>();
        unsigned currentTime = time->GetTimeSinceEpoch();
        unsigned fileTime = fileSystem->GetLastModifiedTime(licenseCachePath_);
        unsigned deltaMinutes = (currentTime - fileTime)/60;
        if (deltaMinutes < 1)
        {
            ATOMIC_LOGINFOF("%u minutes, using cached license", deltaMinutes);
            SendEvent(E_LICENSE_SUCCESS);
            return;
        }
    }

    ATOMIC_LOGINFO("LicenseSystem::RequestServerLicense - requesting verification");

    key_ = key;
    CurlManager* cm = GetSubsystem<CurlManager>();
    String post;
    String id = GenerateMachineID();
    post.AppendWithFormat("key=%s&id=%s", key.CString(), id.CString());

    serverVerification_ = cm->MakeRequest("https://store.atomicgameengine.com/licenses/license_verify.php", post);

    SubscribeToEvent(serverVerification_, E_CURLCOMPLETE, ATOMIC_HANDLER(LicenseSystem, HandleVerification));
}

int LicenseSystem::ParseResponse(const String& response, LicenseParse& parse)
{

    ATOMIC_LOGINFOF("%s", response.CString());

    if (response.StartsWith("AC_ACTIVATIONSEXCEEDED"))
    {
        return 1;
    }

    if (response.StartsWith("AC_IDNOTACTIVATED"))
    {
        return 4;
    }

    if (response.StartsWith("AC_FAILED"))
    {
        return 2;
    }

    if (!response.StartsWith("WINDOWS"))
    {
        ATOMIC_LOGERRORF("Error Parsing Server Response %s", response.CString());
        return 3;
    }

    String codes = response;
    codes.Replace("\n", "");
    codes.Replace("\r", "");

    Vector<String> cvector = codes.Split(' ');

    for (unsigned i = 0; i < cvector.Size(); i++)
    {
        Vector<String> feature = cvector[i].Split('=');
        if (feature.Size() != 2)
            continue;

        if (feature[0] == "WINDOWS")
            parse.licenseWindows_ = !feature[1].StartsWith("0");
        else if (feature[0] == "MAC")
            parse.licenseMac_ = !feature[1].StartsWith("0");
        else if (feature[0] == "ANDROID")
            parse.licenseAndroid_ = !feature[1].StartsWith("0");
        else if (feature[0] == "IOS")
            parse.licenseIOS_ = !feature[1].StartsWith("0");
        else if (feature[0] == "HTML5")
            parse.licenseHTML5_ = !feature[1].StartsWith("0");
        else if (feature[0] == "THREED")
            parse.licenseModule3D_ = !feature[1].StartsWith("0");

    }

    return 0;

}

void LicenseSystem::Activate(const String& key, const LicenseParse& parse)
{
    key_ = key;

    licenseWindows_ = parse.licenseWindows_;
    licenseMac_ = parse.licenseMac_;
    licenseAndroid_ = parse.licenseAndroid_;
    licenseIOS_= parse.licenseIOS_;
    licenseHTML5_= parse.licenseHTML5_;
    licenseModule3D_= parse.licenseModule3D_;

    SaveLicense();
}

bool LicenseSystem::Deactivate()
{
    if (deactivate_.NotNull())
    {
        VariantMap eventData;
        eventData[LicenseDeactivationError::P_MESSAGE] = "LicenseSystem::Deactivate - request already exists";
        SendEvent(E_LICENSE_DEACTIVATIONERROR, eventData);
        return false;
    }

    if (!key_.Length())
    {
        VariantMap eventData;
        eventData[LicenseDeactivationError::P_MESSAGE] = "LicenseSystem::Deactivate - zero length key";
        SendEvent(E_LICENSE_DEACTIVATIONERROR, eventData);
        return false;
    }

    CurlManager* cm = GetSubsystem<CurlManager>();
    String post;
    String id = GenerateMachineID();
    post.AppendWithFormat("key=%s&id=%s", key_.CString(), id.CString());

    deactivate_ = cm->MakeRequest("https://store.atomicgameengine.com/licenses/license_deactivate.php", post);

    SubscribeToEvent(deactivate_, E_CURLCOMPLETE, ATOMIC_HANDLER(LicenseSystem, HandleDeactivate));

    return true;

}

void LicenseSystem::CreateOrUpdateLicenseCache()
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    Time* time = GetSubsystem<Time>();

    if (!fileSystem->FileExists(licenseCachePath_))
    {
        SharedPtr<File> file(new File(context_, licenseCachePath_, FILE_WRITE));
        file->WriteInt(1);
        file->Close();
    }

    fileSystem->SetLastModifiedTime(licenseCachePath_, time->GetTimeSinceEpoch());
}

void LicenseSystem::HandleVerification(StringHash eventType, VariantMap& eventData)
{

    CurlRequest* request = (CurlRequest*) (eventData[CurlComplete::P_CURLREQUEST].GetPtr());

    bool licenseError = false;
    bool resetLicense = false;

    if (serverVerification_.NotNull())
    {
        assert(request == serverVerification_);

        if (serverVerification_->GetError().Length())
        {
            ATOMIC_LOGERRORF("Unable to verify with server: %s", serverVerification_->GetError().CString());
        }
        else
        {
            LicenseParse parse;
            int code = ParseResponse(serverVerification_->GetResponse(), parse);

            if (code == 4)
            {
                // not activated
                resetLicense = true;
                licenseError = true;
            }
            else if (code == 2)
            {
                // something is wrong with the key
                resetLicense = true;
                licenseError = true;

            }
            else if (code == 3)
            {
                // something is wrong on the activation server
                licenseError = true;
            }
            else if (code == 1)
            {
                // exceeded code, should not happen here as we aren't activating
                resetLicense = true;
                licenseError = true;
            }
            else if (code == 0)
            {
                // we should raise an error if there is a mismatch between local and server keys
                // when the local says there are more enabled than server?
                // otherwise, they could be being added

                bool mismatch = false;

                if (parse.licenseWindows_ != licenseWindows_)
                    mismatch = true;

                if (parse.licenseMac_ != licenseMac_)
                    mismatch = true;

                if (parse.licenseWindows_ != licenseWindows_)
                    mismatch = true;

                if (parse.licenseAndroid_ != licenseAndroid_)
                    mismatch = true;

                if (parse.licenseIOS_ != licenseIOS_)
                    mismatch = true;

                if (parse.licenseHTML5_ != licenseHTML5_)
                    mismatch = true;

                if (parse.licenseModule3D_ != licenseModule3D_)
                    mismatch = true;

                if (mismatch)
                {
                    ATOMIC_LOGERROR("License Mismatch, reseting");
                    licenseWindows_ = parse.licenseWindows_;
                    licenseMac_ = parse.licenseMac_;
                    licenseAndroid_ = parse.licenseAndroid_;
                    licenseIOS_= parse.licenseIOS_;
                    licenseHTML5_= parse.licenseHTML5_;
                    licenseModule3D_= parse.licenseModule3D_;

                    SaveLicense();
                }

                CreateOrUpdateLicenseCache();

                SendEvent(E_LICENSE_SUCCESS);
            }

        }

        UnsubscribeFromEvents(serverVerification_);
        serverVerification_ = 0;
    }

    if (resetLicense)
    {
        RemoveLicense();
        ResetLicense();
    }

    if (licenseError)
    {
        ATOMIC_LOGINFO("There was an issue with the atomic-cli activation.  Please reactivate or contact sales@atomicgameengine.com if this problem persists");
        SendEvent(E_LICENSE_ERROR);
    }

}

void LicenseSystem::HandleDeactivate(StringHash eventType, VariantMap& eventData)
{
    CurlRequest* request = (CurlRequest*) (eventData[CurlComplete::P_CURLREQUEST].GetPtr());

    VariantMap eventDataOut;

    if (deactivate_.NotNull())
    {
        assert(request == deactivate_);

        if (deactivate_->GetError().Length())
        {
            String msg = "Deactivation Error:\n";
            msg.AppendWithFormat("Unable to deactivate with server: %s", deactivate_->GetError().CString());

            eventDataOut[LicenseDeactivationError::P_MESSAGE] = msg;
            SendEvent(E_LICENSE_DEACTIVATIONERROR, eventDataOut);
        }
        else
        {
            String response = request->GetResponse();
            if (response.StartsWith("AC_FAILED"))
            {
                String msg = "Deactivation Error:\n";
                msg.AppendWithFormat("Unable to deactivate with server: %s", response.CString());

                eventDataOut[LicenseDeactivationError::P_MESSAGE] = msg;
                SendEvent(E_LICENSE_DEACTIVATIONERROR, eventDataOut);
            }
            else if (response.StartsWith("AC_NOTACTIVATED") || response.StartsWith("AC_SUCCESS"))
            {
                ResetLicense();
                RemoveLicense();
                SendEvent(E_LICENSE_DEACTIVATIONSUCCESS);
            }

        }

        UnsubscribeFromEvents(deactivate_);
        deactivate_ = 0;
    }

}

void LicenseSystem::HandleActivationResult(StringHash eventType, VariantMap& eventData)
{
    VariantMap eventDataOut;

    if (serverActivation_->GetError().Length())
    {
        String errorMessage;
        errorMessage.AppendWithFormat("There was an error contacting the activation server\n\n%s", serverActivation_->GetError().CString());

        eventDataOut[LicenseActivationError::P_MESSAGE] = errorMessage;
        SendEvent(E_LICENSE_ACTIVATIONERROR, eventDataOut);
        return;
    }
    else
    {
        LicenseParse parse;
        int code = ParseResponse(serverActivation_->GetResponse(), parse);

        if (code == 0)
        {
            Activate(key_, parse);
            SendEvent(E_LICENSE_ACTIVATIONSUCCESS);
        }
        else if (code == 1)
        {
            // TODO: check for CLI and prompt to use CLI command to return license
            String message = "Activations Exceeded:\nThis key has 2 activations in use.\n\nPlease return a license from Atomic Editor - Manage License menu on one of these active computers.\n\nIf you are unable to do so, please contact sales@atomicgameengine.com providing the key to reset it";
            eventDataOut[LicenseActivationError::P_MESSAGE] = message;
            SendEvent(E_LICENSE_ACTIVATIONERROR, eventDataOut);

        }
        else if (code == 2)
        {
            String message = "License Error:\nThere was a problem with the license key.\n\nPlease check the key and try again.\n\nIf the problem persists please contact sales@atomicgameengine.com";
            eventDataOut[LicenseActivationError::P_MESSAGE] = message;
            SendEvent(E_LICENSE_ACTIVATIONERROR, eventDataOut);


        }
        else if (code == 3)
        {
            String message ="Activation Server Error:\nThere was an error on the activation server\n\nIf the problem persists please contact sales@atomicgameengine.com";
            eventDataOut[LicenseActivationError::P_MESSAGE] = message;
            SendEvent(E_LICENSE_ACTIVATIONERROR, eventDataOut);
        }

    }

    UnsubscribeFromEvents(serverActivation_);
    serverActivation_ = 0;
}

void LicenseSystem::RequestServerActivation(const String& key)
{
    if (serverActivation_.NotNull())
    {
        ATOMIC_LOGERROR("UIActivation::RequestServerActivation - request already exists");
        return;
    }
    key_ = key;
    CurlManager* cm = GetSubsystem<CurlManager>();
    String post;
    String id = GenerateMachineID();
    post.AppendWithFormat("key=%s&id=%s", key.CString(), id.CString());

    // todo, this should be a verify url (shouldn't auto add id)
    serverActivation_ = cm->MakeRequest("https://store.atomicgameengine.com/licenses/license_activate.php", post);

    SubscribeToEvent(serverActivation_, E_CURLCOMPLETE, ATOMIC_HANDLER(LicenseSystem, HandleActivationResult));
}

bool LicenseSystem::GetSourceBuild()
{

#ifdef ATOMIC_SOURCE_BUILD
    return true;
#else
    return false;
#endif
}

}

// END LICENSE MANAGEMENT
