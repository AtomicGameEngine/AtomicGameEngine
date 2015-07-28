// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "UI/AEWidget.h"

using namespace Atomic;
using namespace tb;

namespace tb
{
class TBLayout;
class TBEditField;
class TBSelectDropdown;
}

namespace AtomicEditor
{

class UIBuildSettingsAndroid: public AEWidget
{
    OBJECT(UIBuildSettingsAndroid);

public:

    UIBuildSettingsAndroid(Context* context);
    virtual ~UIBuildSettingsAndroid();

    bool OnEvent(const TBWidgetEvent &ev);

    void Refresh();

    void StoreSettings();

    //void HandleMessage(StringHash eventType, VariantMap& eventData);

private:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

    void RefreshAndroidTargets();

    String targetOutput_;
    HashMap<unsigned, String> targetLookup_;

    TBEditField* appNameEdit_;
    TBEditField* appPackageEdit_;
    TBEditField* productNameEdit_;
    TBEditField* companyNameEdit_;
    TBSelectDropdown* sdkTargetSelect_;

};

}
