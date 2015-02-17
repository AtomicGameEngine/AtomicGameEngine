// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

// BEGIN LICENSE MANAGEMENT

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/UI/TBUI.h>

#include "Resources/AEResourceOps.h"
#include "AEPreferences.h"

#include "AEEditor.h"
#include "AEEvents.h"
#include "Project/AEProject.h"
#include "Project/ProjectUtils.h"

#include "License/AELicenseSystem.h"
#include "UIEulaAgreement.h"

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

UIEulaAgreement::UIEulaAgreement(Context* context):
    UIModalOpWindow(context)
{
    TBUI* tbui = GetSubsystem<TBUI>();
    window_->SetText("Atomic Game Engine - End User License Agreement");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/eulaagreement.tb.txt");

    eulaCheck_ = window_->GetWidgetByIDAndType<TBCheckBox>(TBIDC("eula_check"));
    assert(eulaCheck_);

    TBEditField* editor_license = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("editor_license"));
    assert(editor_license);

    TBEditField* source_license = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("source_license"));
    assert(source_license);

    TBEditField* thirdparty_license = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("thirdparty_license"));
    assert(thirdparty_license);

    TBEditField* externaltool_license = window_->GetWidgetByIDAndType<TBEditField>(TBIDC("externaltool_license"));
    assert(externaltool_license);


    ResourceCache* cache = GetSubsystem<ResourceCache>();

    SharedPtr<File> file = cache->GetFile("AtomicEditor/eulas/atomic_source_code_eula.txt");
    String text;
    file->ReadText(text);
    source_license->SetText(text.CString());

    file = cache->GetFile("AtomicEditor/eulas/atomic_thirdparty_eula.txt");
    file->ReadText(text);
    thirdparty_license->SetText(text.CString());

    file = cache->GetFile("AtomicEditor/eulas/atomic_external_tools_eula.txt");
    file->ReadText(text);
    externaltool_license->SetText(text.CString());

    file = cache->GetFile("AtomicEditor/eulas/atomic_editor_eula.txt");
    file->ReadText(text);
    editor_license->SetText(text.CString());

    window_->ResizeToFitContent();
    Center();
}


UIEulaAgreement::~UIEulaAgreement()
{
}

bool UIEulaAgreement::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("quit"))
        {
            SendEvent(E_EXITREQUESTED);
            return true;
        }
        else if (ev.target->GetID() == TBIDC("ok"))
        {
            Editor* editor = GetSubsystem<Editor>();

            if (!eulaCheck_->GetValue())
            {
                editor->PostModalInfo("License Agreement", "Please agree to licensing terms and conditions to continue");
                return true;
            }

            SharedPtr<UIEulaAgreement> keepAlive(this);
            GetSubsystem<UIModalOps>()->Hide();
            LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();
            licenseSystem->LicenseAgreementConfirmed();
            return true;
        }


    }


    return false;
}

}

// END LICENSE MANAGEMENT
