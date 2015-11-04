//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Scene/SceneEvents.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Graphics/Camera.h>

#include <Atomic/Graphics/DebugRenderer.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Graphics/Octree.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include <Atomic/Physics/PhysicsWorld.h>

#include <Atomic/Input/Input.h>
#include <Atomic/UI/UI.h>

#include <ToolCore/Assets/AssetDatabase.h>
#include <ToolCore/Assets/Asset.h>

#include "../../EditorMode/AEEditorEvents.h"

#include "SceneEditor3D.h"
#include "SceneEditor3DEvents.h"

using namespace ToolCore;

namespace AtomicEditor
{

SceneEditor3D ::SceneEditor3D(Context* context, const String &fullpath, UITabContainer *container) :
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
          sceneView_->GetCameraNode()->SetWorldPosition(Vector3(-119.073f, 76.1121f, 16.47763f));
          Quaternion q(0.55f, 0.14f,  0.8f, -0.2f);
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

    sceneView_->SetGravity(UI_GRAVITY_ALL);

    rootContentWidget_->AddChild(sceneView_);

    gizmo3D_ = new Gizmo3D(context_);
    gizmo3D_->SetView(sceneView_);
    gizmo3D_->Show();

    SubscribeToEvent(E_UPDATE, HANDLER(SceneEditor3D, HandleUpdate));
    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(SceneEditor3D, HandleEditorActiveNodeChange));

    SubscribeToEvent(E_GIZMOEDITMODECHANGED, HANDLER(SceneEditor3D, HandleGizmoEditModeChanged));
    SubscribeToEvent(E_GIZMOAXISMODECHANGED, HANDLER(SceneEditor3D, HandleGizmoAxisModeChanged));

    // FIXME: Set the size at the end of setup, so all children are updated accordingly
    // future size changes will be handled automatically
    IntRect rect = container_->GetContentRoot()->GetRect();
    rootContentWidget_->SetSize(rect.Width(), rect.Height());

    SubscribeToEvent(E_EDITORPLAYSTARTED, HANDLER(SceneEditor3D, HandlePlayStarted));
    SubscribeToEvent(E_EDITORPLAYSTOPPED, HANDLER(SceneEditor3D, HandlePlayStopped));

    SubscribeToEvent(scene_, E_NODEREMOVED, HANDLER(SceneEditor3D, HandleNodeRemoved));

}

SceneEditor3D::~SceneEditor3D()
{

}

bool SceneEditor3D::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_KEY_UP)
    {
        if (ev.special_key == TB_KEY_DELETE || ev.special_key == TB_KEY_BACKSPACE)
        {
            if (selectedNode_)
            {
                selectedNode_->RemoveAllComponents();
                selectedNode_->Remove();
                selectedNode_ = 0;
            }
        }

    }

    if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (ev.ref_id == TBIDC("copy"))
        {
            if (selectedNode_.NotNull())
            {
                clipboardNode_ = selectedNode_;
            }
        }
        else if (ev.ref_id == TBIDC("paste"))
        {
            if (clipboardNode_.NotNull() && selectedNode_.NotNull())
            {
                SharedPtr<Node> pasteNode(clipboardNode_->Clone());

                VariantMap eventData;
                eventData[EditorActiveNodeChange::P_NODE] = pasteNode;
                SendEvent(E_EDITORACTIVENODECHANGE, eventData);
            }
        }
    }

    if (ev.type == EVENT_TYPE_CLICK)
    {
        SetFocus();

        if (ev.target)
        {
            if (ev.target->GetID() == TBIDC("3d_translate"))
            {
                gizmo3D_->SetEditMode(EDIT_MOVE);
                return false;
            }
            else if (ev.target->GetID() == TBIDC("3d_rotate"))
            {
                gizmo3D_->SetEditMode(EDIT_ROTATE);
                return false;
            }
            else if (ev.target->GetID() == TBIDC("3d_scale"))
            {
                gizmo3D_->SetEditMode(EDIT_SCALE);
                return false;
            }
        }
    }

    return false;
}

void SceneEditor3D::SetFocus()
{
    sceneView_->SetFocus();
}

void SceneEditor3D::SelectNode(Node* node)
{
    selectedNode_ = node;
    if (!node)
        gizmo3D_->Hide();
    else
        gizmo3D_->Show();


}

void SceneEditor3D::HandleNodeRemoved(StringHash eventType, VariantMap& eventData)
{
    Node* node = (Node*) (eventData[NodeRemoved::P_NODE].GetPtr());
    if (node == selectedNode_)
        SelectNode(0);
}


void SceneEditor3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    Vector<Node*> editNodes;
    if (selectedNode_.NotNull())
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

void SceneEditor3D::HandleGizmoEditModeChanged(StringHash eventType, VariantMap& eventData)
{
    EditMode mode = (EditMode) ((int)eventData[GizmoEditModeChanged::P_MODE].GetFloat());
    gizmo3D_->SetEditMode(mode);
}

void SceneEditor3D::HandleGizmoAxisModeChanged(StringHash eventType, VariantMap& eventData)
{
    AxisMode mode = (AxisMode) ((int)eventData[GizmoEditModeChanged::P_MODE].GetFloat());
    gizmo3D_->SetAxisMode(mode);
}


void SceneEditor3D::Close(bool navigateToAvailableResource)
{

    VariantMap data;
    data["Scene"] = scene_;
    SendEvent("EditorSceneClosed", data);

    ResourceEditor::Close(navigateToAvailableResource);
}

bool SceneEditor3D::Save()
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
