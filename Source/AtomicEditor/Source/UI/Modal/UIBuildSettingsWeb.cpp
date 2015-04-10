// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_layout.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/UI/UI.h>

#include "Build/BuildSystem.h"
#include "UIBuildSettingsWeb.h"

namespace AtomicEditor
{

UIBuildSettingsWeb::UIBuildSettingsWeb(Context* context) :
    AEWidget(context)
{
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/buildsettings_html5.tb.txt");

    appNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("app_name"));
    assert(appNameEdit_);

    appPackageEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("app_identifier"));
    assert(appPackageEdit_);

    productNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("product_name"));
    assert(productNameEdit_);

    companyNameEdit_ = delegate_->GetWidgetByIDAndType<TBEditField>(TBIDC("company_name"));
    assert(companyNameEdit_);

}

UIBuildSettingsWeb::~UIBuildSettingsWeb()
{

}

void UIBuildSettingsWeb::Refresh()
{
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    const WebGLSettings& settings = buildSystem->GetBuildSettings()->GetWebGLSettings();

    appNameEdit_->SetText(settings.appName.CString());
    appPackageEdit_->SetText(settings.package.CString());
    productNameEdit_->SetText(settings.productName.CString());
    companyNameEdit_->SetText(settings.companyName.CString());

}

void UIBuildSettingsWeb::StoreSettings()
{
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    WebGLSettings settings;

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


    buildSystem->GetBuildSettings()->SetWebGLSettings(settings);

}


bool UIBuildSettingsWeb::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}



}

