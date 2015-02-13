// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

// BEGIN LICENSE MANAGEMENT

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/UI/TBUI.h>

#include "Resources/AEResourceOps.h"
#include "AEPreferences.h"

#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"
#include "Project/ProjectUtils.h"

#include "AELicenseSystem.h"
#include "UIManageLicense.h"

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

UIManageLicense::UIManageLicense(Context* context):
    UIModalOpWindow(context)

{
    Editor* editor = GetSubsystem<Editor>();
    Project* project = editor->GetProject();

    TBUI* tbui = GetSubsystem<TBUI>();
    window_->SetText("Manage License");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/managelicense.tb.txt");

    window_->ResizeToFitContent();
    Center();

    progressModal_ = new ProgressModal(context_, "Manage License", "");
}


UIManageLicense::~UIManageLicense()
{
}

bool UIManageLicense::OnEvent(const TBWidgetEvent &ev)
{
    Editor* editor = GetSubsystem<Editor>();
    UIModalOps* modelOps = GetSubsystem<UIModalOps>();
    LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("return_activation"))
        {

            if (editor->IsProjectLoaded())
            {
                editor->PostModalError("Close Project", "Please close the current project before deactiving license");
            }
            else
            {
                request_ = licenseSystem->Deactivate();
                SubscribeToEvent(request_, E_CURLCOMPLETE, HANDLER(UIManageLicense, HandleCurlComplete));
                progressModal_->SetMessage("Returning license, please wait...");
                progressModal_->Show();
            }


            return true;
        }
    }

    return false;
}

void UIManageLicense::HandleCurlComplete(StringHash eventType, VariantMap& eventData)
{
    progressModal_->Hide();
}

}

// END LICENSE MANAGEMENT
