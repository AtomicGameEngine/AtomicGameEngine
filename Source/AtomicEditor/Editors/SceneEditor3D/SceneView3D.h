// Portions Copyright (c) 2008-2015 the Urho3D project.

//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

#include <Atomic/UI/UISceneView.h>

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

class SceneView3D: public UISceneView
{
    OBJECT(SceneView3D);

public:

    SceneView3D(Context* context, SceneEditor3D* sceneEditor);
    virtual ~SceneView3D();

    Ray GetCameraRay();

    bool OnEvent(const TBWidgetEvent &ev);

    void SetPitch(float pitch) { pitch_ = pitch; }
    void SetYaw(float yaw) { yaw_ = yaw; }

    void FrameSelection();

    void Enable();
    void Disable();
    bool IsEnabled() { return enabled_; }

    DebugRenderer* GetDebugRenderer() { return debugRenderer_; }
    SceneEditor3D* GetSceneEditor3D() { return sceneEditor_; }

private:

    bool MouseInView();
    bool GetOrbitting();
    bool GetZooming();

    void HandleMouseMove(StringHash eventType, VariantMap& eventData);

    void UpdateDragNode(int mouseX, int mouseY);

    void HandleDragEnded(StringHash eventType, VariantMap& eventData);
    void HandleDragExitWidget(StringHash eventType, VariantMap& eventData);
    void HandleDragEnterWidget(StringHash eventType, VariantMap& eventData);

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    void HandleUIWidgetFocusEscaped(StringHash eventType, VariantMap& eventData);
    void HandleUIUnhandledShortcut(StringHash eventType, VariantMap& eventData);

    void MoveCamera(float timeStep);

    WeakPtr<SceneEditor3D> sceneEditor_;

    float yaw_;
    float pitch_;

    bool mouseLeftDown_;
    bool mouseMoved_;

    bool enabled_;    

    bool cameraMove_;
    float cameraMoveTime_;
    Vector3 cameraMoveStart_;
    Vector3 cameraMoveTarget_;

    SharedPtr<Camera> camera_;
    SharedPtr<DebugRenderer> debugRenderer_;
    SharedPtr<Octree> octree_;

    BoundingBox framedBBox_;

    SharedPtr<Scene> preloadResourceScene_;
    String dragAssetGUID_;
    SharedPtr<Node> dragNode_;

};

}
