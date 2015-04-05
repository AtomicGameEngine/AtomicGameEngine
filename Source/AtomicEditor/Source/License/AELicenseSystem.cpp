// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

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

#include "AtomicEditor.h"
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/Log.h>

#include "AtomicEditor.h"
#include "AEEditor.h"
#include "AEEvents.h"
#include "AELicenseSystem.h"

#include "UI/Modal/UIModalOps.h"
#include "UI/Modal/UIMessageModal.h"

#include "UI/UIMainFrame.h"

#include <Poco/MD5Engine.h>
#include <Poco/File.h>

namespace AtomicEditor
{

LicenseSystem::LicenseSystem(Context* context) :
    Object(context)
    , eulaAgreementConfirmed_(false)

{
    ResetLicense();

    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(LicenseSystem, HandleEditorShutdown));
}

LicenseSystem::~LicenseSystem()
{

}

void LicenseSystem::Initialize()
{

    FileSystem* filesystem = GetSubsystem<FileSystem>();
    String eulaConfirmedFilePath = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    eulaConfirmedFilePath = AddTrailingSlash(eulaConfirmedFilePath);
    eulaConfirmedFilePath += "EulaConfirmed";

    eulaAgreementConfirmed_ = filesystem->FileExists(eulaConfirmedFilePath);

    if (!LoadLicense() || !key_.Length() || !eulaAgreementConfirmed_)
    {
        ResetLicense();
        UIModalOps* ops = GetSubsystem<UIModalOps>();

        if (eulaAgreementConfirmed_)
            ops->ShowActivation();
        else
            ops->ShowEulaAgreement();
    }
    else
    {
        RequestServerVerification(key_);
    }
}

void LicenseSystem::LicenseAgreementConfirmed()
{
    eulaAgreementConfirmed_ = true;

    FileSystem* filesystem = GetSubsystem<FileSystem>();
    String eulaConfirmedFilePath = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    eulaConfirmedFilePath = AddTrailingSlash(eulaConfirmedFilePath);
    eulaConfirmedFilePath += "EulaConfirmed";

    SharedPtr<File> file(new File(context_, eulaConfirmedFilePath, FILE_WRITE));
    file->WriteInt(1);
    file->Close();

    UIModalOps* ops = GetSubsystem<UIModalOps>();
    ops->ShowActivation();
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

    ResetLicense();

    FileSystem* filesystem = GetSubsystem<FileSystem>();
    String licenseFilePath = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    licenseFilePath = AddTrailingSlash(licenseFilePath);
    licenseFilePath += "AtomicLicense";

    if (!filesystem->FileExists(licenseFilePath))
        return false;

    SharedPtr<File> file(new File(context_, licenseFilePath, FILE_READ));

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
    FileSystem* filesystem = GetSubsystem<FileSystem>();
    String licenseFilePath = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    licenseFilePath = AddTrailingSlash(licenseFilePath);

    if (!filesystem->DirExists(licenseFilePath))
    {
        Poco::File dirs(licenseFilePath.CString());
        dirs.createDirectories();
    }

    licenseFilePath += "AtomicLicense";

    SharedPtr<File> file(new File(context_, licenseFilePath, FILE_WRITE));
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

    String licenseFilePath = filesystem->GetAppPreferencesDir("AtomicEditor", "License");
    licenseFilePath = AddTrailingSlash(licenseFilePath);
    licenseFilePath += "AtomicLicense";

    if (filesystem->FileExists(licenseFilePath))
    {
        filesystem->Delete(licenseFilePath);
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
        LOGERROR("LicenseSystem::RequestServerLicense - request already exists");
        return;
    }

    key_ = key;
    CurlManager* cm = GetSubsystem<CurlManager>();
    String post;
    String id = GenerateMachineID();
    post.AppendWithFormat("key=%s&id=%s", key.CString(), id.CString());

    serverVerification_ = cm->MakeRequest("https://store.atomicgameengine.com/licenses/license_verify.php", post);

    SubscribeToEvent(serverVerification_, E_CURLCOMPLETE, HANDLER(LicenseSystem, HandleVerification));
}

int LicenseSystem::ParseResponse(const String& response, LicenseParse& parse)
{

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
        LOGERRORF("Error Parsing Server Response %s", response.CString());
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

bool LicenseSystem::RequestPlatformChange(AEEditorPlatform platform)
{

    if (platform == AE_PLATFORM_HTML5 && LicenseHTML5())
        return true;

    if (platform == AE_PLATFORM_IOS && LicenseIOS())
        return true;

    if (platform == AE_PLATFORM_WINDOWS && LicenseWindows())
        return true;

    if (platform == AE_PLATFORM_MAC && LicenseMac())
        return true;

    if (platform == AE_PLATFORM_ANDROID && LicenseAndroid())
        return true;

#ifdef ATOMIC_PLATFORM_OSX
    if (platform == AE_PLATFORM_MAC)
        return true;
#else
    if (platform == AE_PLATFORM_WINDOWS)
        return true;
#endif

    return false;

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

SharedPtr<CurlRequest>& LicenseSystem::Deactivate()
{
    if (deactivate_.NotNull())
    {
        LOGERROR("LicenseSystem::Deactivate - request already exists");
        return deactivate_;
    }

    if (!key_.Length())
    {
        LOGERROR("LicenseSystem::Deactivate - zero length key");
        return deactivate_;
    }

    CurlManager* cm = GetSubsystem<CurlManager>();
    String post;
    String id = GenerateMachineID();
    post.AppendWithFormat("key=%s&id=%s", key_.CString(), id.CString());

    deactivate_ = cm->MakeRequest("https://store.atomicgameengine.com/licenses/license_deactivate.php", post);

    SubscribeToEvent(deactivate_, E_CURLCOMPLETE, HANDLER(LicenseSystem, HandleDeactivate));

    return deactivate_;

}

void LicenseSystem::HandleVerification(StringHash eventType, VariantMap& eventData)
{

    CurlRequest* request = (CurlRequest*) (eventData[CurlComplete::P_CURLREQUEST].GetPtr());

    if (serverVerification_.NotNull())
    {
        assert(request == serverVerification_);

        if (serverVerification_->GetError().Length())
        {
            LOGERRORF("Unable to verify with server: %s", serverVerification_->GetError().CString());
        }
        else
        {
            LicenseParse parse;
            int code = ParseResponse(serverVerification_->GetResponse(), parse);

            // Not activated
            if (code == 4)
            {

            }
            else if (code == 2)
            {
                // something is wrong with the key
                LOGERRORF("Error with product key");

                RemoveLicense();
                ResetLicense();
                UIModalOps* ops = GetSubsystem<UIModalOps>();
                ops->Hide();
                ops->ShowActivation();

            }
            else if (code == 3)
            {
                // something is wrong on the activation server
                key_ = "";
            }
            else if (code == 1)
            {
                // exceeded code, should not happen here as we aren't activating
                key_ = "";
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
                    LOGERROR("License Mismatch, reseting");
                    licenseWindows_ = parse.licenseWindows_;
                    licenseMac_ = parse.licenseMac_;
                    licenseAndroid_ = parse.licenseAndroid_;
                    licenseIOS_= parse.licenseIOS_;
                    licenseHTML5_= parse.licenseHTML5_;
                    licenseModule3D_= parse.licenseModule3D_;

                    SaveLicense();
                }

                //if (!HasPlatformLicense())
                //{
                //    UIModalOps* ops = GetSubsystem<UIModalOps>();
                //    if (!ops->ModalActive())
                //        ops->ShowPlatformsInfo();

               // }

            }

        }

        UnsubscribeFromEvents(serverVerification_);
        serverVerification_ = 0;
    }

}

void LicenseSystem::HandleDeactivate(StringHash eventType, VariantMap& eventData)
{
    Editor* editor = GetSubsystem<Editor>();

    CurlRequest* request = (CurlRequest*) (eventData[CurlComplete::P_CURLREQUEST].GetPtr());

    if (deactivate_.NotNull())
    {
        assert(request == deactivate_);

        if (deactivate_->GetError().Length())
        {
            String msg;
            msg.AppendWithFormat("Unable to deactivate with server: %s", deactivate_->GetError().CString());
            editor->PostModalError("Deactivation Error", msg);
            LOGERROR(msg);
        }
        else
        {
            String response = request->GetResponse();
            if (response.StartsWith("AC_FAILED"))
            {
                String msg;
                msg.AppendWithFormat("Unable to deactivate with server: %s", response.CString());
                editor->PostModalError("Deactivation Error", msg);
                LOGERROR(msg);
            }
            else if (response.StartsWith("AC_NOTACTIVATED") || response.StartsWith("AC_SUCCESS"))
            {
                ResetLicense();
                RemoveLicense();

                UIModalOps* ops = GetSubsystem<UIModalOps>();
                ops->Hide();
                ops->ShowActivation();
            }

        }

        UnsubscribeFromEvents(deactivate_);
        deactivate_ = 0;
    }

}

void LicenseSystem::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}

// END LICENSE MANAGEMENT
