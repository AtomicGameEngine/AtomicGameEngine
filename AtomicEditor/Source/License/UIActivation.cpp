// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

// BEGIN LICENSE MANAGEMENT

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/UI/TBUI.h>

#include "../Resources/AEResourceOps.h"
#include "../AEPreferences.h"

#include "../AEEditor.h"
#include "../AEEvents.h"
#include "../Project/AEProject.h"
#include "../Project/ProjectUtils.h"

#include "UIActivation.h"
#include "AELicenseSystem.h"

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

UIActivation::UIActivation(Context* context):
    UIModalOpWindow(context),
    licenseKey_(0)
{
    TBUI* tbui = GetSubsystem<TBUI>();
    window_->SetText("Product Activation");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/activation.tb.txt");

    licenseKey_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("license_key"));
    assert(licenseKey_);

    window_->ResizeToFitContent();
    Center();

    progressModal_ = new ProgressModal(context_, "Activation", "Activating, please wait...");
}

UIActivation::~UIActivation()
{

}

void UIActivation::RequestServerActivation(const String& key)
{
    if (serverActivation_.NotNull())
    {
        LOGERROR("UIActivation::RequestServerActivation - request already exists");
        return;
    }

    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();

    key_ = key;
    CurlManager* cm = GetSubsystem<CurlManager>();
    String post;
    String id = licenseSystem->GenerateMachineID();
    post.AppendWithFormat("key=%s&id=%s", key.CString(), id.CString());

    // todo, this should be a verify url (shouldn't auto add id)
    serverActivation_ = cm->MakeRequest("https://store.atomicgameengine.com/licenses/license_activate.php", post);

    SubscribeToEvent(serverActivation_, E_CURLCOMPLETE, HANDLER(UIActivation, HandleCurlComplete));
}


bool UIActivation::OnEvent(const TBWidgetEvent &ev)
{
    Editor* editor = GetSubsystem<Editor>();
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("activate"))
        {
            TBStr _key;
            licenseKey_->GetText(_key);
            String key = _key.CStr();
            key = key.ToUpper().Trimmed();
            if (!licenseSystem->ValidateKey(key))
            {
                editor->PostModalError("Invalid Product Key",
                                       "The key entered is invalid\n\nProduct keys are in the form of ATOMIC-XXXX-XXXX-XXXX-XXXX");
            }
            else
            {
                progressModal_->Show();
                RequestServerActivation(key);
            }

            return true;
        }
    }


    return false;
}

void UIActivation::HandleCurlComplete(StringHash eventType, VariantMap& eventData)
{
    // we want to stay alive through the call, though will be swapped for success modal
    SharedPtr<UIModalOpWindow> keepAlive(this);

    Editor* editor = GetSubsystem<Editor>();
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();


    progressModal_->Hide();

    if (serverActivation_->GetError().Length())
    {
        String errorMessage;
        errorMessage.AppendWithFormat("There was an error contacting the activation server\n\n%s", serverActivation_->GetError().CString());
        editor->PostModalError("Error Contacting Activation Server", errorMessage);
    }
    else
    {
        LicenseSystem::LicenseParse parse;
        int code = licenseSystem->ParseResponse(serverActivation_->GetResponse(), parse);

        if (code == 0)
        {

            licenseSystem->Activate(key_, parse);

            UIModalOps* ops = GetSubsystem<UIModalOps>();
            ops->Hide();
            ops->ShowActivationSuccess();

        }
        else if (code == 1)
        {
            editor->PostModalError("Activations Exceeded",
                            "The key has exceeded the activation limit");
        }
        else if (code == 2)
        {
            editor->PostModalError("License Error",
                            "There was a problem with the license key.\n\nPlease check the key and try again.\n\nIf the problem persists please contact sales@atomicgameengine.com");

        }
        else if (code == 3)
        {
            editor->PostModalError("Activation Server Error",
                            "There was an error on the activation server\n\nIf the problem persists please contact sales@atomicgameengine.com");

        }

    }

    UnsubscribeFromEvents(serverActivation_);
    serverActivation_ = 0;


}

}

// END LICENSE MANAGEMENT
