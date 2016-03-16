// Portions Copyright (c) 2008-2015 the Urho3D project.

//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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
