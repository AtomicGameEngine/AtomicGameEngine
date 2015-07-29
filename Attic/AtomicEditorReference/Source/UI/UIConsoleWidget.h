// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"

using namespace Atomic;
using namespace tb;

namespace tb
{
class TBLayout;
}

namespace AtomicEditor
{

class ListView;

class ConsoleWidget: public AEWidget
{
    OBJECT(ConsoleWidget);

public:

    ConsoleWidget(Context* context);
    virtual ~ConsoleWidget();

    bool OnEvent(const TBWidgetEvent &ev);

    void HandleMessage(StringHash eventType, VariantMap& eventData);

private:

    void HandleEditorShutdown(StringHash eventType, VariantMap& eventData);

    SharedPtr<ListView> consoleList_;
    TBLayout* consolelayout_;

};

}
