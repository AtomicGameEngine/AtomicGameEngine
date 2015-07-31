// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AETypes.h"

#include "UIModalOps.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

#include "UIBuildSettingsWindows.h"
#include "UIBuildSettingsAndroid.h"
#include "UIBuildSettingsWeb.h"
#include "UIBuildSettingsIOS.h"
#include "UIBuildSettingsMac.h"

namespace AtomicEditor
{

class UIBuildSettingsWindows;

class UIBuildSettings: public UIModalOpWindow
{
    OBJECT(UIBuildSettings);

public:

    UIBuildSettings(Context* context);
    virtual ~UIBuildSettings();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    void HandlePlatformChange(StringHash eventType, VariantMap& eventData);

    void RemoveSettingsWidgets();

    void SelectWindowsSettings();
    void SelectAndroidSettings();
    void SelectMacSettings();
    void SelectWebSettings();
    void SelectIOSSettings();

    void StoreSettings();

    void RefreshSettings();

    void UpdateCurrentPlatform(AEEditorPlatform platform);
    void RequestPlatformChange(TBID id);

    TBGenericStringItemSource platformSource_;
    TBSelectList* platformSelect_;
    TBSkinImage* platformIndicator_;

    SharedPtr<UIBuildSettingsWindows> windowsSettings_;
    SharedPtr<UIBuildSettingsAndroid> androidSettings_;
    SharedPtr<UIBuildSettingsWeb> webSettings_;
    SharedPtr<UIBuildSettingsIOS> iosSettings_;
    SharedPtr<UIBuildSettingsMac> macSettings_;

};

}
