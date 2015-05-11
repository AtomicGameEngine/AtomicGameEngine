// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/Log.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Graphics/Camera.h>

#include <Atomic/Graphics/DebugRenderer.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Graphics/Octree.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include <Atomic/Physics/PhysicsWorld.h>

#include "AEEditor.h"
#include "AEEvents.h"

#include <Atomic/Input/Input.h>
#include <Atomic/UI/UI.h>

#include "SceneEditor3D.h"

namespace AtomicEditor
{

SceneEditor3D ::SceneEditor3D(Context* context, const String &fullpath, TBTabContainer *container) :
    ResourceEditor(context, fullpath, container)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    SharedPtr<File> xmlFile = cache->GetFile(fullpath);

    if (GetExtension(fullpath) == ".scene")
        scene_->LoadXML(*xmlFile);
    else
        scene_->Load(*xmlFile);

    scene_->SetUpdateEnabled(false);

    sceneView_ = new SceneView3D(context_, this);

    // EARLY ACCESS
    if (fullpath.Find(String("ToonTown")) != String::NPOS)
    {
          sceneView_->GetCameraNode()->SetWorldPosition(Vector3(-119.073, 76.1121, 16.47763));
          Quaternion q(0.55, 0.14,  0.8, -0.2);
          sceneView_->SetYaw(q.YawAngle());
          sceneView_->SetPitch(q.PitchAngle());
          sceneView_->GetCameraNode()->SetWorldRotation(q);
    }
    else
    {
        Node* playerSpawn = scene_->GetChild("PlayerInfoStart", true);
        if (playerSpawn)
        {
            sceneView_->GetCameraNode()->SetPosition(playerSpawn->GetPosition());
            sceneView_->SetYaw(playerSpawn->GetRotation().EulerAngles().y_);
        }

    }

    TBWidgetDelegate* delegate = sceneView_->GetWidgetDelegate();
    delegate->SetGravity(WIDGET_GRAVITY_ALL);

    rootContentWidget_->AddChild(delegate);

    gizmo3D_ = new Gizmo3D(context_);
    gizmo3D_->SetView(sceneView_);
    gizmo3D_->Show();

    SubscribeToEvent(E_UPDATE, HANDLER(SceneEditor3D, HandleUpdate));
    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(SceneEditor3D, HandleEditorActiveNodeChange));

    // FIXME: Set the size at the end of setup, so all children are updated accordingly
    // future size changes will be handled automatically
    TBRect rect = container_->GetContentRoot()->GetRect();
    rootContentWidget_->SetSize(rect.w, rect.h);

    // TODO: generate this event properly
    VariantMap eventData;
    eventData[EditorActiveSceneChange::P_SCENE] = scene_;
    SendEvent(E_EDITORACTIVESCENECHANGE, eventData);

    SubscribeToEvent(E_EDITORPLAYSTARTED, HANDLER(SceneEditor3D, HandlePlayStarted));
    SubscribeToEvent(E_EDITORPLAYSTOPPED, HANDLER(SceneEditor3D, HandlePlayStopped));

}

SceneEditor3D::~SceneEditor3D()
{

}

bool SceneEditor3D::OnEvent(const TBWidgetEvent &ev)
{

    if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (ev.ref_id == TBIDC("save"))
        {
            File file(context_);
            if (file.Open(fullpath_, FILE_WRITE))
            {
                scene_->SaveXML(file);
                file.Close();
            }

            return true;

        }
    }

    if (ev.type == EVENT_TYPE_CLICK)
    {
        SetFocus();

        if (ev.target)
        {
            if (ev.target->GetID() == TBIDC("3d_translate"))
            {
                gizmo3D_->SetEditMode(Gizmo3D::EDIT_MOVE);
                return false;
            }
            else if (ev.target->GetID() == TBIDC("3d_rotate"))
            {
                gizmo3D_->SetEditMode(Gizmo3D::EDIT_ROTATE);
                return false;
            }
            else if (ev.target->GetID() == TBIDC("3d_scale"))
            {
                gizmo3D_->SetEditMode(Gizmo3D::EDIT_SCALE);
                return false;
            }
        }
    }

    return false;
}

void SceneEditor3D::SetFocus()
{
    sceneView_->GetWidgetDelegate()->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);
}

void SceneEditor3D::SelectNode(Node* node)
{
    selectedNode_ = node;
}

void SceneEditor3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{    
    Vector<Node*> editNodes;
    editNodes.Push(selectedNode_);
    gizmo3D_->Update(editNodes);
}

void SceneEditor3D::HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData)
{
    Node* node = (Node*) (eventData[EditorActiveNodeChange::P_NODE].GetPtr());
    SelectNode(node);
}

void SceneEditor3D::HandlePlayStarted(StringHash eventType, VariantMap& eventData)
{
    sceneView_->Disable();

}

void SceneEditor3D::HandlePlayStopped(StringHash eventType, VariantMap& eventData)
{
    sceneView_->Enable();
}




}
