// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <TurboBadger/tb_widgets_common.h>

#include "../ResourceEditor.h"
#include "SceneView3D.h"
#include "Gizmo3D.h"

using namespace Atomic;
using namespace tb;

namespace Atomic
{
class Scene;
class Node;
class View3D;
class Camera;
class DebugRenderer;
class Octree;

}

namespace AtomicEditor
{

class SceneEditor3D: public ResourceEditor
{
    OBJECT(SceneEditor3D);

public:

    SceneEditor3D(Context* context, const String& fullpath, TBTabContainer* container);

    virtual ~SceneEditor3D();

    bool OnEvent(const TBWidgetEvent &ev);

    void SelectNode(Node* node);

    Scene* GetScene() { return scene_; }
    Gizmo3D* GetGizmo() { return gizmo3D_; }

    void SetFocus();

    virtual bool RequiresInspector() { return true; }


private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData);
    void HandlePlayStarted(StringHash eventType, VariantMap& eventData);
    void HandlePlayStopped(StringHash eventType, VariantMap& eventData);

    SharedPtr<Scene> scene_;

    // TODO: multiple views
    SharedPtr<SceneView3D> sceneView_;

    SharedPtr<Gizmo3D> gizmo3D_;

    WeakPtr<Node> selectedNode_;

};

}
