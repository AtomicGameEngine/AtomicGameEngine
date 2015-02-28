// Portions Copyright (c) 2008-2015 the Urho3D project.

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>

#include "UI/UIView3D.h"

using namespace Atomic;

namespace Atomic
{
class Scene;
class Node;
class Camera;
class DebugRenderer;
class Octree;
}

namespace AtomicEditor
{

class SceneEditor3D;

class SceneView3D: public UIView3D
{
    OBJECT(SceneView3D);

public:

    SceneView3D(Context* context, SceneEditor3D* sceneEditor);
    virtual ~SceneView3D();
    void SelectNode(Node* node);

    Ray GetCameraRay();

    bool OnEvent(const TBWidgetEvent &ev);

    void SetPitch(float pitch) { pitch_ = pitch; }
    void SetYaw(float yaw) { yaw_ = yaw; }

    void Enable();
    void Disable();
    bool IsEnabled() { return enabled_; }

private:

    bool MouseInView();

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    void HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData);

    void DrawNodeDebug(Node* node, DebugRenderer* debug, bool drawNode = true);

    void MoveCamera(float timeStep);

    WeakPtr<SceneEditor3D> sceneEditor_;

    float yaw_;
    float pitch_;

    bool mouseLeftDown_;
    bool mouseMoved_;

    bool enabled_;

    SharedPtr<Camera> camera_;
    SharedPtr<DebugRenderer> debugRenderer_;
    SharedPtr<Octree> octree_;
    SharedPtr<Node> selectedNode_;

};

}
