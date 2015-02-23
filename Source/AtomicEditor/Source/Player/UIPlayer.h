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

    void SetView(Scene* scene, Camera* camera);

    bool OnEvent(const TBWidgetEvent &ev);

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    SharedPtr<View3D> view3D_;
    WeakPtr<AEPlayer> aePlayer_;

};

}
