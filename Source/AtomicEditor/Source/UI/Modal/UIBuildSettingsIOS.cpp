// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <TurboBadger/tb_layout.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_select.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/MemoryBuffer.h>
#include <Atomic/UI/TBUI.h>

#include "AEEditor.h"
#include "AEEvents.h"
#include "AEPreferences.h"
#include "Project/ProjectUtils.h"
#include "Subprocess/AESubprocessSystem.h"

#include "Build/BuildSystem.h"

#include "UIBuildSettingsIOS.h"

namespace AtomicEditor
{

UIBuildSettingsIOS::UIBuildSettingsIOS(Context* context) :
    AEWidget(context)
{
    TBUI* tbui = GetSubsystem<TBUI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/buildsettings_ios.tb.txt");

    appNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("app_name"));
    assert(appNameEdit_);

    appPackageEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("app_package"));
    assert(appPackageEdit_);

    productNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("product_name"));
    assert(productNameEdit_);

    companyNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("company_name"));
    assert(companyNameEdit_);

    provisionPath_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("provision_path"));
    assert(provisionPath_);

    Refresh();
}

UIBuildSettingsIOS::~UIBuildSettingsIOS()
{

}


void UIBuildSettingsIOS::StoreSettings()
{
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    IOSBuildSettings settings;

    TBStr text;
    appNameEdit_->GetText(text);
    settings.appName = text.CStr();
    text.Clear();

    appPackageEdit_->GetText(text);
    settings.package = text.CStr();
    text.Clear();

    productNameEdit_->GetText(text);
    settings.productName = text.CStr();
    text.Clear();

    companyNameEdit_->GetText(text);
    settings.companyName = text.CStr();
    text.Clear();

    provisionPath_->GetText(text);
    settings.provisionFile = text.CStr();
    text.Clear();

    buildSystem->GetBuildSettings()->SetIOSSettings(settings);

}

void UIBuildSettingsIOS::Refresh()
{
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    const IOSBuildSettings& settings = buildSystem->GetBuildSettings()->GetIOSSettings();

    provisionPath_->SetText(settings.provisionFile.CString());
    appNameEdit_->SetText(settings.appName.CString());
    appPackageEdit_->SetText(settings.package.CString());
    productNameEdit_->SetText(settings.productName.CString());
    companyNameEdit_->SetText(settings.companyName.CString());
}

bool UIBuildSettingsIOS::OnEvent(const TBWidgetEvent &ev)
{
    Editor* editor = context_->GetSubsystem<Editor>();
    ProjectUtils* utils = context_->GetSubsystem<ProjectUtils>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("choose_provision_path"))
        {
            String path = utils->GetMobileProvisionPath();
            provisionPath_->SetText(path.CString());
            return true;
        }
    }

    return false;

}

}

