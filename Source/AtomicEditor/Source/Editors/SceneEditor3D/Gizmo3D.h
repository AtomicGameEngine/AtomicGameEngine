
// Portions Copyright (c) 2008-2015 the Urho3D project.

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/Math/Ray.h>
#include <Atomic/Scene/Scene.h>

#include <Atomic/Graphics/StaticModel.h>

using namespace Atomic;

namespace Atomic
{
    class Node;
}

namespace AtomicEditor
{

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

    /*
    void Update(Ray cameraRay, float scale, bool drag)
    {
        // Do not select when UI has modal element
        if (ui.HasModalElement())
        {
            selected = false;
            return;
        }

        Vector3 closest = cameraRay.ClosestPoint(axisRay);
        Vector3 projected = axisRay.Project(closest);
        d = axisRay.Distance(closest);
        t = (projected - axisRay.origin).DotProduct(axisRay.direction);

        // Determine the sign of d from a plane that goes through the camera position to the axis
        Plane axisPlane(cameraNode.position, axisRay.origin, axisRay.origin + axisRay.direction);
        if (axisPlane.Distance(closest) < 0.0)
            d = -d;

        // Update selected status only when not dragging
        if (!drag)
        {
            selected = Abs(d) < axisMaxD * scale && t >= -axisMaxD * scale && t <= axisMaxT * scale;
            lastT = t;
            lastD = d;
        }
    }
    */

    void Moved()
    {
        lastT_ = t_;
        lastD_ = d_;
    }
};

class Gizmo3D: public Object
{
    OBJECT(Gizmo3D);

public:

    enum GizmoMode
    {
        EDIT_MOVE
    };

    Gizmo3D(Context* context);
    virtual ~Gizmo3D();

    Node* GetGizmoNode() { return gizmoNode_; }

private:

    SharedPtr<Node> gizmoNode_;

    WeakPtr<StaticModel> gizmo_;

    Gizmo3DAxis gizmoAxisX_;
    Gizmo3DAxis gizmoAxisY_;
    Gizmo3DAxis gizmoAxisZ_;

    GizmoMode lastGizmoMode_;

};

}

