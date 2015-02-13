// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"

using namespace Atomic;
using namespace tb;

// replaces resource frame while playing

namespace AtomicEditor
{

class PlayerWidget: public AEWidget
{
    OBJECT(PlayerWidget);

public:

    PlayerWidget(Context* context);
    virtual ~PlayerWidget();

    bool OnEvent(const TBWidgetEvent &ev);

};

}
