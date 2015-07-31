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

class UIManageLicense: public UIModalOpWindow
{
    OBJECT(UIManageLicense);

public:

    UIManageLicense(Context* context);
    virtual ~UIManageLicense();
    bool OnEvent(const TBWidgetEvent &ev);

private:

    void HandleCurlComplete(StringHash eventType, VariantMap& eventData);

    SharedPtr<ProgressModal> progressModal_;
    SharedPtr<CurlRequest> request_;

};

}

// END LICENSE MANAGEMENT
