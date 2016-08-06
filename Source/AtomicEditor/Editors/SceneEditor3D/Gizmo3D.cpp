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

#include <Atomic/Graphics/Model.h>
#include <Atomic/Graphics/Material.h>
#include <Atomic/Graphics/Octree.h>
#include <Atomic/Resource/ResourceCache.h>

#include <Atomic/Input/Input.h>

#include "SceneEditor3DEvents.h"
#include "SceneSelection.h"
#include "SceneEditor3D.h"
#include "Gizmo3D.h"

namespace AtomicEditor
{

Gizmo3D::Gizmo3D(Context* context) : Object(context),
    dragging_(false),
    cloning_(false),
    startClone_(false)
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

    gizmo_->SetMaterial(0, cache->GetResource<Material>("AtomicEditor/Materials/RedUnlit.xml"));
    gizmo_->SetMaterial(1, cache->GetResource<Material>("AtomicEditor/Materials/GreenUnlit.xml"));
    gizmo_->SetMaterial(2, cache->GetResource<Material>("AtomicEditor/Materials/BlueUnlit.xml"));

}

Gizmo3D::~Gizmo3D()
{

}

void Gizmo3D::SetView(SceneView3D* view3D)
{
    view3D_ = view3D;
    scene_ = view3D->GetScene();
    camera_ = view3D->GetCameraNode()->GetComponent<Camera>();
    selection_ = view3D_->GetSceneEditor3D()->GetSelection();
    assert(camera_.NotNull());
}

void Gizmo3D::Position()
{
    Vector3 center(0, 0, 0);
    bool containsScene = false;

    Vector<SharedPtr<Node>>& editNodes = selection_->GetNodes();

    for (unsigned i = 0; i < editNodes.Size(); ++i)
    {
        // Scene's transform should not be edited, so hide gizmo if it is included
        if (editNodes[i] == scene_)
        {
            containsScene = true;
            break;
        }
        center += editNodes[i]->GetWorldPosition();
    }

    if (editNodes.Empty() || containsScene)
    {
        Hide();
        return;
    }

    center /= editNodes.Size();
    gizmoNode_->SetPosition(center);

    if (axisMode_ == AXIS_WORLD || editNodes.Size() > 1)
        gizmoNode_->SetRotation(Quaternion());
    else
        gizmoNode_->SetRotation(editNodes[0]->GetWorldRotation());

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

void Gizmo3D::Update()
{
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
        return;
    }

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Input* input = GetSubsystem<Input>();

    Vector<SharedPtr<Node>>& editNodes = selection_->GetNodes();
    Ray cameraRay = view3D_->GetCameraRay();
    float scale = gizmoNode_->GetScale().x_;

    // Clones an object when it's dragged/rotated/scaled while holding down Shift
    if (startClone_ && !cloning_)
    {
        cloning_ = true;
        selection_->Copy();
        selection_->Paste();
    }

    // Recalculate axes only when not left-dragging
    bool drag = input->GetMouseButtonDown(MOUSEB_LEFT);// && (Abs(input->GetMouseMoveX()) > 3 || Abs(input->GetMouseMoveY()) > 3);
    if (!drag)
    {
        if (dragging_)
        {
            scene_->SendEvent(E_SCENEEDITEND);
            dragging_ = false;
        }

        startClone_ = false;
        cloning_ = false;

        CalculateGizmoAxes();
    }

    gizmoAxisX_.Update(cameraRay, scale, drag, camera_->GetNode());
    gizmoAxisY_.Update(cameraRay, scale, drag, camera_->GetNode());
    gizmoAxisZ_.Update(cameraRay, scale, drag, camera_->GetNode());

    if (!editNodes.Size() || editNodes[0] == scene_)
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

    if (drag && Selected())
        Drag();

}

bool Gizmo3D::MoveEditNodes(Vector3 adjust)
{
    bool moved = false;

    Input* input = GetSubsystem<Input>();

#ifdef ATOMIC_PLATFORM_OSX
    bool moveSnap = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#else
    bool moveSnap = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#endif

    Vector<SharedPtr<Node>>& editNodes = selection_->GetNodes();

    if (adjust.Length() > M_EPSILON)
    {
        for (unsigned i = 0; i < editNodes.Size(); ++i)
        {
            if (moveSnap)
            {
                float moveStepScaled = snapTranslationX_;
                adjust.x_ = floorf(adjust.x_ / moveStepScaled + 0.5) * moveStepScaled;
                moveStepScaled = snapTranslationY_;
                adjust.y_ = floorf(adjust.y_ / moveStepScaled + 0.5) * moveStepScaled;
                moveStepScaled = snapTranslationZ_;
                adjust.z_ = floorf(adjust.z_ / moveStepScaled + 0.5) * moveStepScaled;
            }

            Node* node = editNodes[i];
            Vector3 nodeAdjust = adjust;
            if (axisMode_ == AXIS_LOCAL && editNodes.Size() == 1)
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

    Input* input = GetSubsystem<Input>();

#ifdef ATOMIC_PLATFORM_OSX
    bool rotateSnap = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#else
    bool rotateSnap = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#endif

    Vector<SharedPtr<Node>>& editNodes = selection_->GetNodes();

    if (rotateSnap)
    {
        float rotateStepScaled = snapRotation_;
        adjust.x_ = floorf(adjust.x_ / rotateStepScaled + 0.5) * rotateStepScaled;
        adjust.y_ = floorf(adjust.y_ / rotateStepScaled + 0.5) * rotateStepScaled;
        adjust.z_ = floorf(adjust.z_ / rotateStepScaled + 0.5) * rotateStepScaled;
    }

    if (adjust.Length() > M_EPSILON)
    {
        moved = true;

        for (unsigned i = 0; i < editNodes.Size(); ++i)
        {
            Node* node = editNodes[i];

            Quaternion rotQuat(adjust.x_, adjust.y_, adjust.z_);
            if (axisMode_ == AXIS_LOCAL && editNodes.Size() == 1)
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

    Input* input = GetSubsystem<Input>();

#ifdef ATOMIC_PLATFORM_OSX
    bool scaleSnap = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#else
    bool scaleSnap = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#endif

    Vector<SharedPtr<Node>>& editNodes = selection_->GetNodes();

    if (adjust.Length() > M_EPSILON)
    {
        for (unsigned i = 0; i < editNodes.Size(); ++i)
        {
            Node* node = editNodes[i];

            Vector3 scale = node->GetScale();
            Vector3 oldScale = scale;

            if (!scaleSnap)
                scale += adjust;
            else
            {
                float scaleStepScaled = snapScale_;
                if (adjust.x_ != 0)
                {
                    scale.x_ += adjust.x_ * scaleStepScaled;
                    scale.x_ = floorf(scale.x_ / scaleStepScaled + 0.5) * scaleStepScaled;
                }
                if (adjust.y_ != 0)
                {
                    scale.y_ += adjust.y_ * scaleStepScaled;
                    scale.y_ = floorf(scale.y_ / scaleStepScaled + 0.5) * scaleStepScaled;
                }
                if (adjust.z_ != 0)
                {
                    scale.z_ += adjust.z_ * scaleStepScaled;
                    scale.z_ = floorf(scale.z_ / scaleStepScaled + 0.5) * scaleStepScaled;
                }
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

    SendEvent(E_GIZMOMOVED);
}


void Gizmo3D::Drag()
{
    bool moved = false;

    dragging_ = true;

    float scale = gizmoNode_->GetScale().x_;

    Input* input = GetSubsystem<Input>();

    //Activates cloning when any of these modes is used while holding shift
    if (editMode_ == EDIT_MOVE ||
        editMode_ == EDIT_ROTATE ||
        editMode_ == EDIT_SCALE)
    {
        startClone_ = input->GetKeyDown(KEY_SHIFT);
    }

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

void Gizmo3D::SetAxisMode(AxisMode mode)
{
    axisMode_ = mode;
}

void Gizmo3D::SetEditMode(EditMode mode)
{
    editMode_ = mode;
}

void Gizmo3D::Hide()
{
    gizmoAxisX_.selected_ = gizmoAxisY_.selected_ = gizmoAxisZ_.selected_ = false;
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

float Gizmo3D::GetSnapTranslationX() const
{
    return snapTranslationX_;
}

float Gizmo3D::GetSnapTranslationY() const
{
    return snapTranslationY_;
}

float Gizmo3D::GetSnapTranslationZ() const
{
    return snapTranslationZ_;
}

float Gizmo3D::GetSnapRotation() const
{
    return snapRotation_;
}

float Gizmo3D::GetSnapScale() const
{
    return snapScale_;
}

void Gizmo3D::SetSnapTranslationX(float value)
{
    snapTranslationX_ = value;
}

void Gizmo3D::SetSnapTranslationY(float value)
{
    snapTranslationY_ = value;
}

void Gizmo3D::SetSnapTranslationZ(float value)
{
    snapTranslationZ_ = value;
}

void Gizmo3D::SetSnapRotation(float value)
{
    snapRotation_ = value;
}

void Gizmo3D::SetSnapScale(float value)
{
    snapScale_ = value;
}

}
