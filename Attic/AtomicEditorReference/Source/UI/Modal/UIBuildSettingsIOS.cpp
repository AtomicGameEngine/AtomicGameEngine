// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <PugiXml/src/pugixml.hpp>
#include <TurboBadger/tb_layout.h>
#include <TurboBadger/tb_editfield.h>
#include <TurboBadger/tb_select.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/MemoryBuffer.h>
#include <Atomic/UI/UI.h>

#include "AEEditor.h"
#include "AEEvents.h"
#include "AEPreferences.h"
#include "Project/ProjectUtils.h"
#include "Subprocess/AESubprocessSystem.h"

#include "Build/BuildSystem.h"
#include "Build/BuildIOSUtils.h"

#include "UIBuildSettingsIOS.h"

namespace AtomicEditor
{

UIBuildSettingsIOS::UIBuildSettingsIOS(Context* context) :
    AEWidget(context)
{
    UI* tbui = GetSubsystem<UI>();
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

    applicationIDPrefix_ = delegate_->GetWidgetByIDAndType<TBTextField>(TBIDC("appid_prefix"));
    assert(applicationIDPrefix_);

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

    applicationIDPrefix_->GetText(text);
    settings.appidPrefix = text.CStr();
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
    applicationIDPrefix_->SetText(settings.appidPrefix.CString());
}

bool UIBuildSettingsIOS::ParseProvisionData(const String& provisionFile)
{
#if defined(ATOMIC_PLATFORM_WINDOWS) || defined(ATOMIC_PLATFORM_LINUX)

    return false;

#else
    String pdata = GetMobileProvisionData(provisionFile.CString());

    if (!pdata.Length())
        return false;

    pugi::xml_document doc;

    if (!doc.load(pdata.CString()))
    {
        return false;
    }

    String AppIDName;
    String ApplicationIdentifierPrefix;

    pugi::xml_node dict = doc.document_element().child("dict");

    for (pugi::xml_node key = dict.child("key"); key; key = key.next_sibling("key"))
    {
        String keyName = key.child_value();

        if (keyName == "AppIDName")
        {
            pugi::xml_node value = key.next_sibling();
            if (!strcmp(value.name(), "string"))
                AppIDName = value.child_value();
        }
        else if (keyName == "ApplicationIdentifierPrefix")
        {
            pugi::xml_node array = key.next_sibling();
            if (!strcmp(array.name(), "array"))
            {
                pugi::xml_node value = array.first_child();
                if (!strcmp(value.name(), "string"))
                    ApplicationIdentifierPrefix = value.child_value();
            }

        }
    }

    if (!ApplicationIdentifierPrefix.Length())
        return false;

    applicationIDPrefix_->SetText(ApplicationIdentifierPrefix.CString());
    provisionPath_->SetText(provisionFile.CString());

    return true;
#endif

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
            if (path.Length())
            {
                if (!ParseProvisionData(path))
                    editor->PostModalError("Mobile Provision Error", "Could not parse mobile provision");
                else
                    StoreSettings();
            }
            return true;
        }
    }

    return false;

}

}
