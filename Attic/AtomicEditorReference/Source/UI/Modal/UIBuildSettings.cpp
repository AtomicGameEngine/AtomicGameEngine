// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_window.h>
#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_editfield.h>

#include <Atomic/Core/Context.h>
#include <Atomic/UI/UI.h>

#include "AEEvents.h"
#include "AEEditor.h"
#include "Resources/AEResourceOps.h"

#include "UIBuildSettings.h"

// BEGIN LICENSE MANAGEMENT
#include "License/AELicenseSystem.h"
// END LICENSE MANAGEMENT

namespace AtomicEditor
{

// UIBuildSettings------------------------------------------------

UIBuildSettings::UIBuildSettings(Context* context):
    UIModalOpWindow(context),
    windowsSettings_(new UIBuildSettingsWindows(context)),
    androidSettings_(new UIBuildSettingsAndroid(context)),
    webSettings_(new UIBuildSettingsWeb(context)),
    iosSettings_(new UIBuildSettingsIOS(context)),
    macSettings_(new UIBuildSettingsMac(context)),
    platformIndicator_(0)
{
    UI* tbui = GetSubsystem<UI>();
    window_->SetText("Atomic Player - Build Settings");
    tbui->LoadResourceFile(window_->GetContentRoot(), "AtomicEditor/editor/ui/buildsettings.tb.txt");

    TBLayout* platformcontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("platformcontainer"));
    assert(platformcontainer);

    platformSelect_ = new TBSelectList();
    platformSelect_->SetSource(&platformSource_);
    LayoutParams lp;
    lp.min_h = 370;
    lp.max_h = 370;
    lp.min_w = 160;
    platformSelect_->SetLayoutParams(lp);
    platformSelect_->SetGravity(WIDGET_GRAVITY_ALL);

    TBGenericStringItem* item = new TBGenericStringItem("Windows");
    item->SetSkinImage(TBIDC("LogoWindows"));
    item->id = TBIDC("WindowsBuildSettings");
    platformSource_.AddItem(item);
    item = new TBGenericStringItem("Mac");
    item->SetSkinImage(TBIDC("LogoMac"));
    item->id = TBIDC("MacBuildSettings");
    platformSource_.AddItem(item);
    item = new TBGenericStringItem("WebGL");
    item->SetSkinImage(TBIDC("LogoHTML5"));
    item->id = TBIDC("WebGLBuildSettings");
    platformSource_.AddItem(item);
    item = new TBGenericStringItem("Android");
    item->SetSkinImage(TBIDC("LogoAndroid"));
    item->id = TBIDC("AndroidBuildSettings");
    platformSource_.AddItem(item);
    item = new TBGenericStringItem("iOS");
    item->SetSkinImage(TBIDC("LogoIOS"));
    item->id = TBIDC("iOSBuildSettings");
    platformSource_.AddItem(item);

    platformSelect_->SetValue(-1);
    platformSelect_->SetValue(0);

    platformcontainer->AddChild(platformSelect_);

    platformIndicator_ = delegate_->GetWidgetByIDAndType<TBSkinImage>(TBIDC("current_platform_indicator"));
    assert(platformIndicator_);

    RefreshSettings();

    Editor* editor = GetSubsystem<Editor>();
    AEEditorPlatform platform = editor->GetCurrentPlatform();
    UpdateCurrentPlatform(platform);

    // windows settings
    if (platform == AE_PLATFORM_WINDOWS)
        SelectWindowsSettings();
    else if (platform == AE_PLATFORM_MAC)
        SelectMacSettings();
    else if (platform == AE_PLATFORM_ANDROID)
        SelectAndroidSettings();
    else if (platform == AE_PLATFORM_HTML5)
        SelectWebSettings();
    else if (platform == AE_PLATFORM_IOS)
        SelectIOSSettings();

    window_->ResizeToFitContent();
    Center();

    SubscribeToEvent(E_PLATFORMCHANGE, HANDLER(UIBuildSettings, HandlePlatformChange));

}

void UIBuildSettings::RemoveSettingsWidgets()
{
    TBLayout* settingscontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("settingscontainer"));
    assert(settingscontainer);

    if (windowsSettings_->GetWidgetDelegate()->GetParent() == settingscontainer)
        settingscontainer->RemoveChild(windowsSettings_->GetWidgetDelegate());

    if (androidSettings_->GetWidgetDelegate()->GetParent() == settingscontainer)
        settingscontainer->RemoveChild(androidSettings_->GetWidgetDelegate());

    if (webSettings_->GetWidgetDelegate()->GetParent() == settingscontainer)
        settingscontainer->RemoveChild(webSettings_->GetWidgetDelegate());

    if (iosSettings_->GetWidgetDelegate()->GetParent() == settingscontainer)
        settingscontainer->RemoveChild(iosSettings_->GetWidgetDelegate());

    if (macSettings_->GetWidgetDelegate()->GetParent() == settingscontainer)
        settingscontainer->RemoveChild(macSettings_->GetWidgetDelegate());

}

void UIBuildSettings::SelectWindowsSettings()
{
    RemoveSettingsWidgets();

    TBLayout* settingscontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("settingscontainer"));
    assert(settingscontainer);
    settingscontainer->AddChild(windowsSettings_->GetWidgetDelegate());
    windowsSettings_->Refresh();
    platformSelect_->SetValue(0);

}

void UIBuildSettings::SelectWebSettings()
{
    RemoveSettingsWidgets();

    TBLayout* settingscontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("settingscontainer"));
    assert(settingscontainer);
    settingscontainer->AddChild(webSettings_->GetWidgetDelegate());
    webSettings_->Refresh();
    platformSelect_->SetValue(2);

}


void UIBuildSettings::SelectAndroidSettings()
{
    RemoveSettingsWidgets();

    TBLayout* settingscontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("settingscontainer"));
    assert(settingscontainer);
    settingscontainer->AddChild(androidSettings_->GetWidgetDelegate());
    androidSettings_->Refresh();

    platformSelect_->SetValue(3);

}

void UIBuildSettings::SelectIOSSettings()
{
    RemoveSettingsWidgets();

    TBLayout* settingscontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("settingscontainer"));
    assert(settingscontainer);
    settingscontainer->AddChild(iosSettings_->GetWidgetDelegate());
    iosSettings_->Refresh();

    platformSelect_->SetValue(4);

}


void UIBuildSettings::SelectMacSettings()
{
    RemoveSettingsWidgets();

    RemoveSettingsWidgets();

    TBLayout* settingscontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("settingscontainer"));
    assert(settingscontainer);
    settingscontainer->AddChild(macSettings_->GetWidgetDelegate());
    macSettings_->Refresh();

    platformSelect_->SetValue(1);

}


UIBuildSettings::~UIBuildSettings()
{

    platformSelect_->SetSource(NULL);
    platformSource_.DeleteAllItems();
}

void UIBuildSettings::RequestPlatformChange(TBID id)
{
    Editor* editor = GetSubsystem<Editor>();

    AEEditorPlatform platform = AE_PLATFORM_UNDEFINED;

    if (id == TBIDC("WindowsBuildSettings"))
    {
       platform = AE_PLATFORM_WINDOWS;
    }
    else if (id == TBIDC("MacBuildSettings"))
    {
        platform = AE_PLATFORM_MAC;
    }
    else if (id == TBIDC("WebGLBuildSettings"))
    {
        platform = AE_PLATFORM_HTML5;
    }
    else if (id == TBIDC("AndroidBuildSettings"))
    {
        platform = AE_PLATFORM_ANDROID;
    }
    else if (id == TBIDC("iOSBuildSettings"))
    {
        platform = AE_PLATFORM_IOS;
    }

    if (platform == AE_PLATFORM_UNDEFINED)
    {
#ifdef ATOMIC_PLATFORM_OSX
       platform = AE_PLATFORM_MAC;
#else
       platform = AE_PLATFORM_WINDOWS;
#endif
    }

    editor->RequestPlatformChange(platform);
}

void UIBuildSettings::RefreshSettings()
{
    androidSettings_->Refresh();
    iosSettings_->Refresh();
    macSettings_->Refresh();
    windowsSettings_->Refresh();
    webSettings_->Refresh();
}

void UIBuildSettings::StoreSettings()
{
    androidSettings_->StoreSettings();
    iosSettings_->StoreSettings();
    macSettings_->StoreSettings();
    windowsSettings_->StoreSettings();
    webSettings_->StoreSettings();

    Editor* editor = GetSubsystem<Editor>();
    editor->SaveProject();

}

bool UIBuildSettings::OnEvent(const TBWidgetEvent &ev)
{    
    UIModalOps* ops = GetSubsystem<UIModalOps>();

    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.ref_id == TBIDC("WindowsBuildSettings"))
        {
            SelectWindowsSettings();
            return true;
        }
        else if (ev.ref_id == TBIDC("AndroidBuildSettings"))
        {
            SelectAndroidSettings();
            return true;
        }
        else if (ev.ref_id == TBIDC("WebGLBuildSettings"))
        {
            SelectWebSettings();
            return true;
        }
        else if (ev.ref_id == TBIDC("iOSBuildSettings"))
        {
            SelectIOSSettings();
            return true;
        }
        else if (ev.ref_id == TBIDC("MacBuildSettings"))
        {
            SelectMacSettings();
            return true;
        }

        else if (ev.target->GetID() == TBIDC("set_current_platform"))
        {
// BEGIN LICENSE MANAGEMENT

            TBID id = platformSelect_->GetSelectedItemID();
            LicenseSystem* licenseSystem = GetSubsystem<LicenseSystem>();
            if (licenseSystem->IsStandardLicense())
            {
                if (id == TBIDC("WebGLBuildSettings") || id == TBIDC("AndroidBuildSettings") || id == TBIDC("iOSBuildSettings"))
                {
                    SharedPtr<UIBuildSettings> keepAlive(this);
                    UIModalOps* ops = GetSubsystem<UIModalOps>();
                    ops->Hide();
                    ops->ShowPlatformsInfo();
                    return true;
                }

            }

#ifdef ATOMIC_PLATFORM_WINDOWS

                if (id == TBIDC("iOSBuildSettings"))
                {
                    Editor* editor = GetSubsystem<Editor>();
                    editor->PostModalInfo("MacOSX Required", "iOS platform requires MacOSX Editor");
                    return true;
                }
#endif


            RequestPlatformChange(id);
            return true;
// END LICENSE MANAGEMENT

        }
        else if (ev.target->GetID() == TBIDC("ok"))
        {
            StoreSettings();
            ops->Hide();
            return true;
        }
        else if (ev.target->GetID() == TBIDC("build"))
        {
            StoreSettings();
            SharedPtr<UIBuildSettings> keepAlive(this);
            ops->Hide();
            ops->ShowBuild();
            return true;
        }
        else if (ev.target->GetID() == TBIDC("cancel"))
        {
            ops->Hide();
            return true;
        }
    }

    return false;
}

void UIBuildSettings::UpdateCurrentPlatform(AEEditorPlatform platform)
{
    if (platform == AE_PLATFORM_MAC)
        platformIndicator_->SetSkinBg(TBIDC("LogoMac"));
    else if (platform == AE_PLATFORM_WINDOWS)
        platformIndicator_->SetSkinBg(TBIDC("LogoWindows"));
    else if (platform == AE_PLATFORM_ANDROID)
        platformIndicator_->SetSkinBg(TBIDC("LogoAndroid"));
    else if (platform == AE_PLATFORM_HTML5)
        platformIndicator_->SetSkinBg(TBIDC("LogoHTML5"));
    else if (platform == AE_PLATFORM_IOS)
        platformIndicator_->SetSkinBg(TBIDC("LogoIOS"));

}

void UIBuildSettings::HandlePlatformChange(StringHash eventType, VariantMap& eventData)
{
    using namespace PlatformChange;
    AEEditorPlatform platform = (AEEditorPlatform) eventData[P_PLATFORM].GetUInt();
    UpdateCurrentPlatform(platform);

}


}
