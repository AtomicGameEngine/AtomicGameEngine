// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine


#pragma once

#include "UI/Modal/UIModalOps.h"
#include "UI/UIView3D.h"

#include "AEPlayer.h"

namespace Atomic
{
class Scene;
class Camera;
}

namespace AtomicEditor
{

/// Player modal

class UIPlayer: public UIModalOpWindow
{
    OBJECT(UIPlayer);

public:

    UIPlayer(Context* context);
    virtual ~UIPlayer();

    bool OnEvent(const TBWidgetEvent &ev);
    const IntVector2& GetPlayerSize() { return playerSize_; }

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    WeakPtr<AEPlayer> aePlayer_;
    IntVector2 playerSize_;
};

}
