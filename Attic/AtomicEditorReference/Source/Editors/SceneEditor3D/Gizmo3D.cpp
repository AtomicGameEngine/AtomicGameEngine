// Portions Copyright (c) 2008-2015 the Urho3D project.

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Atomic3D/Model.h>
#include <Atomic/Graphics/Material.h>
#include <Atomic/Graphics/Octree.h>
#include <Atomic/Resource/ResourceCache.h>

#include <Atomic/Input/Input.h>

#include "Gizmo3D.h"

namespace AtomicEditor
{


Gizmo3D::Gizmo3D(Context* context) : Object(context)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    gizmoNode_ = new Node(context_);
    gizmo_ = gizmoNode_->CreateComponent<StaticModel>();
    gizmo_->SetModel(cache->GetResource<Model>("AtomicEditor/Models/Axes.mdl"));

    gizmo_->SetEnabled(false);
    gizmo_->SetViewMask(0x80000000); // Editor raycasts use viewmask 0x7fffffff
    gizmo_->SetOccludee(false);

    axisMode_ = AXIS_LOCAL;

    gizmoAxisX_.lastSelected_ = false;
    gizmoAxisY_.lastSelected_ = false;
    gizmoAxisZ_.lastSelected_ = false;

    editMode_ = EDIT_MOVE;
    lastEditMode_ = EDIT_SELECT;
}

Gizmo3D::~Gizmo3D()
{

}

void Gizmo3D::SetView(SceneView3D* view3D)
{
    view3D_ = view3D;
    scene_ = view3D->GetScene();
    camera_ = view3D->GetCameraNode()->GetComponent<Camera>();
    assert(camera_.NotNull());
}

void Gizmo3D::Position()
{
    Vector3 center(0, 0, 0);
    bool containsScene = false;

    for (unsigned i = 0; i < editNodes_->Size(); ++i)
    {
        // Scene's transform should not be edited, so hide gizmo if it is included
        if (editNodes_->At(i) == scene_)
        {
            containsScene = true;
            break;
        }
        center += editNodes_->At(i)->GetWorldPosition();
    }

    if (editNodes_->Empty() || containsScene)
    {
        Hide();
        return;
    }

    center /= editNodes_->Size();
    gizmoNode_->SetPosition(center);

    if (axisMode_ == AXIS_WORLD || editNodes_->Size() > 1)
        gizmoNode_->SetRotation(Quaternion());
    else
        gizmoNode_->SetRotation(editNodes_->At(0)->GetWorldRotation());

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    if (editMode_ != lastEditMode_)
    {
        switch (editMode_)
        {
        case EDIT_MOVE:
            gizmo_->SetModel(cache->GetResource<Model>("AtomicEditor/Models/Axes.mdl"));
            break;

        case EDIT_ROTATE:
            gizmo_->SetModel(cache->GetResource<Model>("AtomicEditor/Models/RotateAxes.mdl"));
            break;

        case EDIT_SCALE:
            gizmo_->SetModel(cache->GetResource<Model>("AtomicEditor/Models/ScaleAxes.mdl"));
            break;

        default:
            break;
        }

        lastEditMode_ = editMode_;
    }

    bool orbiting = false;
    if ((editMode_ != EDIT_SELECT && !orbiting) && !gizmo_->IsEnabled())
        Show();
    else if ((editMode_ == EDIT_SELECT || orbiting) && gizmo_->IsEnabled())
        Hide();

    if (gizmo_->IsEnabled())
    {
        float scale = 0.1f / camera_->GetZoom();

        if (camera_->IsOrthographic())
            scale *= camera_->GetOrthoSize();
        else
            scale *= (camera_->GetView() * gizmoNode_->GetPosition()).z_;

        gizmoNode_->SetScale(Vector3(scale, scale, scale));

    }
}

void Gizmo3D::Update(Vector<Node *> &editNodes)
{
    editNodes_ = &editNodes;

    Use();
    Position();


}

void Gizmo3D::CalculateGizmoAxes()
{
    gizmoAxisX_.axisRay_ = Ray(gizmoNode_->GetPosition(), gizmoNode_->GetRotation() * Vector3(1, 0, 0));
    gizmoAxisY_.axisRay_ = Ray(gizmoNode_->GetPosition(), gizmoNode_->GetRotation() * Vector3(0, 1, 0));
    gizmoAxisZ_.axisRay_ = Ray(gizmoNode_->GetPosition(), gizmoNode_->GetRotation() * Vector3(0, 0, 1));
}

void Gizmo3D::Use()
{
    if (gizmo_.Null() || !gizmo_->IsEnabled() || editMode_ == EDIT_SELECT)
    {
        //StoreGizmoEditActions();
        //previousGizmoDrag = false;
        return;
    }

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Input* input = GetSubsystem<Input>();

    Ray cameraRay = view3D_->GetCameraRay();
    float scale = gizmoNode_->GetScale().x_;

    // Recalculate axes only when not left-dragging
    bool drag = input->GetMouseButtonDown(MOUSEB_LEFT);// && (Abs(input->GetMouseMoveX()) > 3 || Abs(input->GetMouseMoveY()) > 3);
    if (!drag)
        CalculateGizmoAxes();

    gizmoAxisX_.Update(cameraRay, scale, drag, camera_->GetNode());
    gizmoAxisY_.Update(cameraRay, scale, drag, camera_->GetNode());
    gizmoAxisZ_.Update(cameraRay, scale, drag, camera_->GetNode());

    if (!editNodes_->Size() || editNodes_->At(0) == scene_)
    {
        gizmoAxisX_.selected_ = gizmoAxisY_.selected_ = gizmoAxisZ_.selected_ = false;
        // this just forces an update
        gizmoAxisX_.lastSelected_ = gizmoAxisY_.lastSelected_ = gizmoAxisZ_.lastSelected_ = true;
    }


    if (gizmoAxisX_.selected_ != gizmoAxisX_.lastSelected_)
    {
        gizmo_->SetMaterial(0, cache->GetResource<Material>(
                                gizmoAxisX_.selected_ ?
                                    "AtomicEditor/Materials/BrightRedUnlit.xml" : "AtomicEditor/Materials/RedUnlit.xml"));

        gizmoAxisX_.lastSelected_ = gizmoAxisX_.selected_;
    }

    if (gizmoAxisY_.selected_ != gizmoAxisY_.lastSelected_)
    {
        gizmo_->SetMaterial(1, cache->GetResource<Material>(
                                gizmoAxisY_.selected_ ?
                                    "AtomicEditor/Materials/BrightGreenUnlit.xml" : "AtomicEditor/Materials/GreenUnlit.xml"));

        gizmoAxisY_.lastSelected_ = gizmoAxisY_.selected_;
    }
    if (gizmoAxisZ_.selected_ != gizmoAxisZ_.lastSelected_)
    {
        gizmo_->SetMaterial(2, cache->GetResource<Material>(
                                gizmoAxisZ_.selected_ ?
                                    "AtomicEditor/Materials/BrightBlueUnlit.xml" : "AtomicEditor/Materials/BlueUnlit.xml"));

        gizmoAxisZ_.lastSelected_ = gizmoAxisZ_.selected_;
    }

    if (drag)
        Drag();

}

bool Gizmo3D::MoveEditNodes(Vector3 adjust)
{
    bool moved = false;

    if (adjust.Length() > M_EPSILON)
    {
        for (unsigned i = 0; i < editNodes_->Size(); ++i)
        {
            /*
            if (moveSnap)
            {
                float moveStepScaled = moveStep * snapScale;
                adjust.x = Floor(adjust.x / moveStepScaled + 0.5) * moveStepScaled;
                adjust.y = Floor(adjust.y / moveStepScaled + 0.5) * moveStepScaled;
                adjust.z = Floor(adjust.z / moveStepScaled + 0.5) * moveStepScaled;
            }
            */

            Node* node = editNodes_->At(i);
            Vector3 nodeAdjust = adjust;
            if (axisMode_ == AXIS_LOCAL && editNodes_->Size() == 1)
                nodeAdjust = node->GetWorldRotation() * nodeAdjust;

            Vector3 worldPos = node->GetWorldPosition();
            Vector3 oldPos = node->GetPosition();

            worldPos += nodeAdjust;

            if (!node->GetParent())
                node->SetPosition(worldPos);
            else
                node->SetPosition(node->GetParent()->WorldToLocal(worldPos));

            if (node->GetPosition() != oldPos)
                moved = true;
        }
    }

    return moved;


}

bool Gizmo3D::RotateEditNodes(Vector3 adjust)
{
    bool moved = false;

    /*
    if (rotateSnap)
    {
        float rotateStepScaled = rotateStep * snapScale;
        adjust.x = Floor(adjust.x / rotateStepScaled + 0.5) * rotateStepScaled;
        adjust.y = Floor(adjust.y / rotateStepScaled + 0.5) * rotateStepScaled;
        adjust.z = Floor(adjust.z / rotateStepScaled + 0.5) * rotateStepScaled;
    }
    */

    if (adjust.Length() > M_EPSILON)
    {
        moved = true;

        for (unsigned i = 0; i < editNodes_->Size(); ++i)
        {
            Node* node = editNodes_->At(i);
            Quaternion rotQuat(adjust.x_, adjust.y_, adjust.z_);
            if (axisMode_ == AXIS_LOCAL && editNodes_->Size() == 1)
                node->SetRotation(node->GetRotation() * rotQuat);
            else
            {
                Vector3 offset = node->GetWorldPosition() - gizmoAxisX_.axisRay_.origin_;
                if (node->GetParent() && node->GetParent()->GetWorldRotation() != Quaternion(1, 0, 0, 0))
                    rotQuat = node->GetParent()->GetWorldRotation().Inverse() * rotQuat * node->GetParent()->GetWorldRotation();
                node->SetRotation(rotQuat * node->GetRotation());
                Vector3 newPosition = gizmoAxisX_.axisRay_.origin_ + rotQuat * offset;
                if (node->GetParent())
                    newPosition = node->GetParent()->WorldToLocal(newPosition);
                node->SetPosition(newPosition);
            }
        }
    }

    return moved;
}

bool Gizmo3D::ScaleEditNodes(Vector3 adjust)
{
    bool moved = false;

    if (adjust.Length() > M_EPSILON)
    {
        for (unsigned i = 0; i < editNodes_->Size(); ++i)
        {
            Node* node = editNodes_->At(i);

            Vector3 scale = node->GetScale();
            Vector3 oldScale = scale;

            if (true)//!scaleSnap)
                scale += adjust;
            else
            {
                /*
                float scaleStepScaled = scaleStep * snapScale;
                if (adjust.x != 0)
                {
                    scale.x += adjust.x * scaleStepScaled;
                    scale.x = Floor(scale.x / scaleStepScaled + 0.5) * scaleStepScaled;
                }
                if (adjust.y != 0)
                {
                    scale.y += adjust.y * scaleStepScaled;
                    scale.y = Floor(scale.y / scaleStepScaled + 0.5) * scaleStepScaled;
                }
                if (adjust.z != 0)
                {
                    scale.z += adjust.z * scaleStepScaled;
                    scale.z = Floor(scale.z / scaleStepScaled + 0.5) * scaleStepScaled;
                }
                */
            }

            if (scale != oldScale)
                moved = true;

            node->SetScale(scale);
        }
    }

    return moved;
}

void Gizmo3D::Moved()
{
    gizmoAxisX_.Moved();
    gizmoAxisY_.Moved();
    gizmoAxisZ_.Moved();
}


void Gizmo3D::Drag()
{
    bool moved = false;

    float scale = gizmoNode_->GetScale().x_;

    if (editMode_ == EDIT_MOVE)
    {
        Vector3 adjust(0, 0, 0);
        if (gizmoAxisX_.selected_)
            adjust += Vector3(1, 0, 0) * (gizmoAxisX_.t_ - gizmoAxisX_.lastT_);
        if (gizmoAxisY_.selected_)
            adjust += Vector3(0, 1, 0) * (gizmoAxisY_.t_ - gizmoAxisY_.lastT_);
        if (gizmoAxisZ_.selected_)
            adjust += Vector3(0, 0, 1) * (gizmoAxisZ_.t_ - gizmoAxisZ_.lastT_);

        moved = MoveEditNodes(adjust);
    }
    else if (editMode_ == EDIT_ROTATE)
    {
        const float rotSensitivity = 50.0;

        Vector3 adjust(0, 0, 0);
        if (gizmoAxisX_.selected_)
            adjust.x_ = (gizmoAxisX_.d_ - gizmoAxisX_.lastD_) * rotSensitivity / scale;
        if (gizmoAxisY_.selected_)
            adjust.y_ = -(gizmoAxisY_.d_ - gizmoAxisY_.lastD_) * rotSensitivity / scale;
        if (gizmoAxisZ_.selected_)
            adjust.z_ = (gizmoAxisZ_.d_ - gizmoAxisZ_.lastD_) * rotSensitivity / scale;

        moved = RotateEditNodes(adjust);
    }
    else if (editMode_ == EDIT_SCALE)
    {
        Vector3 adjust(0, 0, 0);
        if (gizmoAxisX_.selected_)
            adjust += Vector3(1, 0, 0) * (gizmoAxisX_.t_ - gizmoAxisX_.lastT_);
        if (gizmoAxisY_.selected_)
            adjust += Vector3(0, 1, 0) * (gizmoAxisY_.t_ - gizmoAxisY_.lastT_);
        if (gizmoAxisZ_.selected_)
            adjust += Vector3(0, 0, 1) * (gizmoAxisZ_.t_ - gizmoAxisZ_.lastT_);

        // Special handling for uniform scale: use the unmodified X-axis movement only
        if (editMode_ == EDIT_SCALE && gizmoAxisX_.selected_ && gizmoAxisY_.selected_ && gizmoAxisZ_.selected_)
        {
            float x = gizmoAxisX_.t_ - gizmoAxisX_.lastT_;
            adjust = Vector3(x, x, x);
        }

        moved = ScaleEditNodes(adjust);
    }

    if (moved)
    {
        Moved();
        //UpdateNodeAttributes();
        //needGizmoUndo = true;
    }

}

void Gizmo3D::SetEditMode(EditMode mode)
{
    editMode_ = mode;
}

void Gizmo3D::Hide()
{
    gizmo_->SetEnabled(false);
}

void Gizmo3D::Show()
{
    if (scene_.Null())
        return;

    gizmo_->SetEnabled(true);

    Octree* octree = scene_->GetComponent<Octree>();
    if (!octree)
        return;

    octree->AddManualDrawable(gizmo_);

}

}
