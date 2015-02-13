// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

// BEGIN LICENSE MANAGEMENT

#pragma once

#include "UI/Modal/UIModalOps.h"
#include "UI/Modal/UIProgressModal.h"
#include "Net/CurlManager.h"

#include <TurboBadger/tb_select.h>
#include <TurboBadger/tb_select_item.h>

namespace AtomicEditor
{

class UIActivation: public UIModalOpWindow
{
    OBJECT(UIActivation);

public:

    UIActivation(Context* context);
    virtual ~UIActivation();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    void RequestServerActivation(const String& key);

    void HandleCurlComplete(StringHash eventType, VariantMap& eventData);

    TBEditField* licenseKey_;
    String key_;

    SharedPtr<CurlRequest> serverActivation_;

    SharedPtr<ProgressModal> progressModal_;


};

}

// END LICENSE MANAGEMENT
