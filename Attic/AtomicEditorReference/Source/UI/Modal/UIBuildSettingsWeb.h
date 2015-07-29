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
class TBTextField;
class TBSelectDropdown;
}

namespace AtomicEditor
{

class UIBuildSettingsWeb: public AEWidget
{
    OBJECT(UIBuildSettingsWeb);

public:

    UIBuildSettingsWeb(Context* context);
    virtual ~UIBuildSettingsWeb();

    bool OnEvent(const TBWidgetEvent &ev);

    void Refresh();
    void StoreSettings();

    //void HandleMessage(StringHash eventType, VariantMap& eventData);

private:

    TBEditField* appNameEdit_;
    TBEditField* appPackageEdit_;
    TBEditField* productNameEdit_;
    TBEditField* companyNameEdit_;


};

}
