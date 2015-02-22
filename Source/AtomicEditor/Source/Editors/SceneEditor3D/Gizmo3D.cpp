// Portions Copyright (c) 2008-2015 the Urho3D project.

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/Graphics/Model.h>
#include <Atomic/Graphics/Material.h>
#include <Atomic/Resource/ResourceCache.h>

#include "Gizmo3D.h"

namespace AtomicEditor
{


Gizmo3D::Gizmo3D(Context* context) : Object(context)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    gizmoNode_ = new Node(context_);
    gizmo_ = gizmoNode_->CreateComponent<StaticModel>();
    gizmo_->SetModel(cache->GetResource<Model>("AtomicEditor/Models/Axes.mdl"));
    gizmo_->SetMaterial(0, cache->GetResource<Material>("Materials/Editor/RedUnlit.xml"));
    gizmo_->SetMaterial(1, cache->GetResource<Material>("Materials/Editor/GreenUnlit.xml"));
    gizmo_->SetMaterial(2, cache->GetResource<Material>("Materials/Editor/BlueUnlit.xml"));


    gizmo_->SetEnabled(false);
    gizmo_->SetViewMask(0x80000000); // Editor raycasts use viewmask 0x7fffffff
    gizmo_->SetOccludee(false);

    gizmoAxisX_.lastSelected_ = false;
    gizmoAxisY_.lastSelected_ = false;
    gizmoAxisZ_.lastSelected_ = false;
    lastGizmoMode_ = EDIT_MOVE;

}

Gizmo3D::~Gizmo3D()
{

}

}
