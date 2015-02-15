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

class UIBuildSettingsIOS: public AEWidget
{
    OBJECT(UIBuildSettingsIOS);

public:

    UIBuildSettingsIOS(Context* context);
    virtual ~UIBuildSettingsIOS();

    bool OnEvent(const TBWidgetEvent &ev);

    void Refresh();

    void StoreSettings();

private:

    bool ParseProvisionData(const String& provisionFile);

    String targetOutput_;
    HashMap<unsigned, String> targetLookup_;

    TBEditField* appNameEdit_;
    TBEditField* appPackageEdit_;
    TBEditField* productNameEdit_;
    TBEditField* companyNameEdit_;

    TBEditField* provisionPath_;
    TBTextField* applicationIDPrefix_;

};

}
