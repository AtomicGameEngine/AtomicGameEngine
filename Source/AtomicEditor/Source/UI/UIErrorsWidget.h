// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"
#include "../Player/AEPlayer.h"

using namespace Atomic;
using namespace tb;

namespace tb
{
class TBLayout;
}

namespace AtomicEditor
{

class ListView;

class ErrorsWidget: public AEWidget
{
    OBJECT(ErrorsWidget);

public:

    ErrorsWidget(Context* context);
    virtual ~ErrorsWidget();

    void UpdateErrors();

    bool OnEvent(const TBWidgetEvent &ev);

private:

    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    Vector<AEPlayerError> errors_;
    SharedPtr<ListView> errorList_;
    TBLayout* errorlayout_;

};

}
