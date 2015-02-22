// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

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

class SceneEditor3D;

class SceneView3D: public Object
{
    OBJECT(SceneView3D);

public:

    SceneView3D(Context* context, SceneEditor3D* sceneEditor);

    virtual ~SceneView3D();

    void SelectNode(Node* node);

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    void HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData);

    void MoveCamera(float timeStep);

    WeakPtr<SceneEditor3D> sceneEditor_;
    WeakPtr<Scene> scene_;

    // Camera
    WeakPtr<Node> cameraNode_;
    WeakPtr<Camera> camera_;

    float yaw_;
    float pitch_;

    SharedPtr<View3D> view3D_;

    WeakPtr<DebugRenderer> debugRenderer_;
    WeakPtr<Octree> octree_;
    WeakPtr<Node> selectedNode_;

};

}
