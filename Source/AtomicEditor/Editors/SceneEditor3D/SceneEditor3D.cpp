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

#include <ToolCore/ToolSystem.h>
#include <ToolCore/Project/Project.h>
#include <ToolCore/Project/ProjectEvents.h>
#include <ToolCore/Project/ProjectUserPrefs.h>
#include <ToolCore/Assets/AssetDatabase.h>
#include <ToolCore/Assets/Asset.h>


#include "../../EditorMode/AEEditorEvents.h"

#include "SceneEditor3D.h"
#include "SceneEditHistory.h"
#include "SceneEditor3DEvents.h"

using namespace ToolCore;

namespace AtomicEditor
{

SceneEditor3D ::SceneEditor3D(Context* context, const String &fullpath, UITabContainer *container) :
    ResourceEditor(context, fullpath, container)
{

    // store a local reference to user project prefs
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();
    userPrefs_ = project->GetUserPrefs();

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
    UpdateGizmoSnapSettings();

    SubscribeToEvent(E_UPDATE, HANDLER(SceneEditor3D, HandleUpdate));
    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(SceneEditor3D, HandleEditorActiveNodeChange));

    SubscribeToEvent(E_GIZMOEDITMODECHANGED, HANDLER(SceneEditor3D, HandleGizmoEditModeChanged));
    SubscribeToEvent(E_GIZMOAXISMODECHANGED, HANDLER(SceneEditor3D, HandleGizmoAxisModeChanged));

    // FIXME: Set the size at the end of setup, so all children are updated accordingly
    // future size changes will be handled automatically
    IntRect rect = container_->GetContentRoot()->GetRect();
    rootContentWidget_->SetSize(rect.Width(), rect.Height());

    SubscribeToEvent(E_PROJECTUSERPREFSAVED, HANDLER(SceneEditor3D, HandleUserPrefSaved));

    SubscribeToEvent(E_EDITORPLAYSTARTED, HANDLER(SceneEditor3D, HandlePlayStarted));
    SubscribeToEvent(E_EDITORPLAYSTOPPED, HANDLER(SceneEditor3D, HandlePlayStopped));

    SubscribeToEvent(scene_, E_NODEADDED, HANDLER(SceneEditor3D, HandleNodeAdded));
    SubscribeToEvent(scene_, E_NODEREMOVED, HANDLER(SceneEditor3D, HandleNodeRemoved));

    SubscribeToEvent(scene_, E_SCENEEDITSCENEMODIFIED, HANDLER(SceneEditor3D, HandleSceneEditSceneModified));

    editHistory_ = new SceneEditHistory(context_, scene_);

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
                VariantMap editData;
                editData[SceneEditNodeAddedRemoved::P_SCENE] = scene_;
                editData[SceneEditNodeAddedRemoved::P_NODE] = selectedNode_;
                editData[SceneEditNodeAddedRemoved::P_ADDED] = false;
                scene_->SendEvent(E_SCENEEDITNODEADDEDREMOVED, editData);

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

                VariantMap editData;
                editData[SceneEditNodeAddedRemoved::P_SCENE] = scene_;
                editData[SceneEditNodeAddedRemoved::P_NODE] = pasteNode;
                editData[SceneEditNodeAddedRemoved::P_ADDED] = true;

                scene_->SendEvent(E_SCENEEDITNODEADDEDREMOVED, editData);
            }
        }
        else if (ev.ref_id == TBIDC("close"))
        {
            RequestClose();
            return true;
        }
        else if (ev.ref_id == TBIDC("undo"))
        {
            Undo();
            return true;
        }
        else if (ev.ref_id == TBIDC("redo"))
        {
            Redo();
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

void SceneEditor3D::HandleNodeAdded(StringHash eventType, VariantMap& eventData)
{
    // Node does not have values set here

    //Node* node =  static_cast<Node*>(eventData[NodeAdded::P_NODE].GetPtr());
    //LOGINFOF("Node Added: %s", node->GetName().CString());
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
    EditMode mode = (EditMode) (eventData[GizmoEditModeChanged::P_MODE].GetInt());
    gizmo3D_->SetEditMode(mode);
}

void SceneEditor3D::HandleGizmoAxisModeChanged(StringHash eventType, VariantMap& eventData)
{
    AxisMode mode = (AxisMode) (eventData[GizmoAxisModeChanged::P_MODE].GetInt());
    bool toggle = eventData[GizmoAxisModeChanged::P_TOGGLE].GetBool();
    if (toggle)
    {
        AxisMode mode = gizmo3D_->GetAxisMode() == AXIS_WORLD ? AXIS_LOCAL : AXIS_WORLD;
        VariantMap neventData;
        neventData[GizmoAxisModeChanged::P_MODE] = (int) mode;
        SendEvent(E_GIZMOAXISMODECHANGED, neventData);
    }
    else
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

    SetModified(false);

    return true;

}

void SceneEditor3D::Undo()
{
    editHistory_->Undo();
}

void SceneEditor3D::Redo()
{
    editHistory_->Redo();
}

void SceneEditor3D::HandleSceneEditSceneModified(StringHash eventType, VariantMap& eventData)
{
    SetModified(true);    
}

void SceneEditor3D::HandleUserPrefSaved(StringHash eventType, VariantMap& eventData)
{
    UpdateGizmoSnapSettings();
}

void SceneEditor3D::GetSelectionBoundingBox(BoundingBox& bbox)
{
    bbox.Clear();

    if (selectedNode_.Null())
        return;

    // TODO: Adjust once multiple selection is in
    if (selectedNode_.Null())
        return;

    // Get all the drawables, which define the bounding box of the selection
    PODVector<Drawable*> drawables;
    selectedNode_->GetDerivedComponents<Drawable>(drawables, true);

    if (!drawables.Size())
        return;

    // Calculate the combined bounding box of all drawables
    for (unsigned i = 0; i < drawables.Size(); i++  )
    {
        Drawable* drawable = drawables[i];
        bbox.Merge(drawable->GetWorldBoundingBox());
    }


}

void SceneEditor3D::UpdateGizmoSnapSettings()
{
    gizmo3D_->SetSnapTranslationX(userPrefs_->GetSnapTranslationX());
    gizmo3D_->SetSnapTranslationY(userPrefs_->GetSnapTranslationY());
    gizmo3D_->SetSnapTranslationZ(userPrefs_->GetSnapTranslationZ());
    gizmo3D_->SetSnapRotation(userPrefs_->GetSnapRotation());
    gizmo3D_->SetSnapScale(userPrefs_->GetSnapScale());

}

}
