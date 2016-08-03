
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
#include <Atomic/Math/MathDefs.h>
#include <Atomic/Math/Ray.h>
#include <Atomic/Scene/Scene.h>

#include <Atomic/Graphics/StaticModel.h>
#include <Atomic/Graphics/Camera.h>


#include "SceneView3D.h"

using namespace Atomic;

namespace Atomic
{
    class Node;
}

namespace AtomicEditor
{

class SceneSelection;

class Gizmo3DAxis
{
public:

    Ray axisRay_;
    bool selected_;
    bool lastSelected_;
    float t_;
    float d_;
    float lastT_;
    float lastD_;

    Gizmo3DAxis()
    {
        selected_ = false;
        lastSelected_ = false;
        t_ = 0.0;
        d_ = 0.0;
        lastT_ = 0.0;
        lastD_ = 0.0;
    }

    void Update(Ray cameraRay, float scale, bool drag, Node* cameraNode)
    {
        const float axisMaxD = 0.1f;
        const float axisMaxT = 1.0f;

        Vector3 closest = cameraRay.ClosestPoint(axisRay_);
        Vector3 projected = axisRay_.Project(closest);
        d_ = axisRay_.Distance(closest);
        t_ = (projected - axisRay_.origin_).DotProduct(axisRay_.direction_);

        // Determine the sign of d from a plane that goes through the camera position to the axis
        Plane axisPlane(cameraNode->GetPosition(), axisRay_.origin_, axisRay_.origin_ + axisRay_.direction_);
        if (axisPlane.Distance(closest) < 0.0)
            d_ = -d_;

        // Update selected status only when not dragging
        if (!drag)
        {
            selected_ = Atomic::Abs(d_) < axisMaxD * scale && t_ >= -axisMaxD * scale && t_ <= axisMaxT * scale;
            lastT_ = t_;
            lastD_ = d_;
        }
    }

    void Moved()
    {
        lastT_ = t_;
        lastD_ = d_;
    }
};

enum EditMode
{
    EDIT_SELECT,
    EDIT_MOVE,
    EDIT_ROTATE,
    EDIT_SCALE
};

enum AxisMode
{
    AXIS_WORLD = 0,
    AXIS_LOCAL
};

class Gizmo3D: public Object
{
    ATOMIC_OBJECT(Gizmo3D, Object);

public:

    Gizmo3D(Context* context);
    virtual ~Gizmo3D();

    void SetView(SceneView3D* view3D);

    void SetAxisMode(AxisMode mode);
    AxisMode GetAxisMode() const { return axisMode_; }

    void SetEditMode(EditMode mode);

    bool Selected()
    {
        return gizmoAxisX_.selected_ || gizmoAxisY_.selected_ || gizmoAxisZ_.selected_;
    }

    void Show();
    void Hide();
    void Update();

    Node* GetGizmoNode() { return gizmoNode_; }

    float GetSnapTranslationX() const;
    float GetSnapTranslationY() const;
    float GetSnapTranslationZ() const;
    float GetSnapRotation() const;
    float GetSnapScale() const;

    void SetSnapTranslationX(float value);
    void SetSnapTranslationY(float value);
    void SetSnapTranslationZ(float value);
    void SetSnapRotation(float value);
    void SetSnapScale(float value);

private:

    void Position();
    void Use();
    void Drag();
    void Moved();
    void CalculateGizmoAxes();

    bool MoveEditNodes(Vector3 adjust);
    bool RotateEditNodes(Vector3 adjust);
    bool ScaleEditNodes(Vector3 adjust);

    SharedPtr<Node> gizmoNode_;

    WeakPtr<SceneView3D> view3D_;
    WeakPtr<Scene> scene_;
    WeakPtr<Camera> camera_;
    WeakPtr<StaticModel> gizmo_;
    WeakPtr<SceneSelection> selection_;

    Gizmo3DAxis gizmoAxisX_;
    Gizmo3DAxis gizmoAxisY_;
    Gizmo3DAxis gizmoAxisZ_;

    EditMode editMode_;
    EditMode lastEditMode_;

    AxisMode axisMode_;

    bool dragging_;
    bool cloning_;
    bool startClone_;

    // snap settings
    float snapTranslationX_;
    float snapTranslationY_;
    float snapTranslationZ_;
    float snapRotation_;
    float snapScale_;

};

}
