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
#include <ToolCore/Assets/SceneImporter.h>


#include "../../EditorMode/AEEditorEvents.h"

#include "SceneEditor3D.h"
#include "SceneSelection.h"
#include "SceneEditHistory.h"
#include "SceneEditor3DEvents.h"

using namespace ToolCore;

namespace AtomicEditor
{

Vector<WeakPtr<SceneEditor3D>> SceneEditor3D::sceneEditors_;

SceneEditor3D::SceneEditor3D(Context* context, const String &fullpath, UITabContainer *container) :
    ResourceEditor(context, fullpath, container),
    cubemapRenderCount_(0)
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

    selection_ = new SceneSelection(context, this);
    sceneView_ = new SceneView3D(context_, this);
    editHistory_ = new SceneEditHistory(context, this);

    AssetDatabase* assetDB = GetSubsystem<AssetDatabase>();
    Asset* sceneAsset = assetDB->GetAssetByPath(fullpath);

    if (sceneAsset)
    {
        sceneImporter_ = static_cast<SceneImporter*>(sceneAsset->GetImporter());
        sceneView_->GetCameraNode()->SetWorldPosition(sceneImporter_->GetSceneCamPosition());
        sceneView_->SetPitch(sceneImporter_->GetSceneCamRotation().PitchAngle());
        sceneView_->SetYaw(sceneImporter_->GetSceneCamRotation().YawAngle());
    }
    else
    {
        ATOMIC_LOGERRORF("SceneEditor3D::SceneEditor3D - Unable to get scene asset");
    }

    sceneView_->SetGravity(UI_GRAVITY_ALL);

    rootContentWidget_->AddChild(sceneView_);

    gizmo3D_ = new Gizmo3D(context_);
    gizmo3D_->SetView(sceneView_);
    gizmo3D_->Show();
    UpdateGizmoSnapSettings();

    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(SceneEditor3D, HandleUpdate));

    SubscribeToEvent(E_GIZMOEDITMODECHANGED, ATOMIC_HANDLER(SceneEditor3D, HandleGizmoEditModeChanged));
    SubscribeToEvent(E_GIZMOAXISMODECHANGED, ATOMIC_HANDLER(SceneEditor3D, HandleGizmoAxisModeChanged));

    // FIXME: Set the size at the end of setup, so all children are updated accordingly
    // future size changes will be handled automatically
    IntRect rect = container_->GetContentRoot()->GetRect();
    rootContentWidget_->SetSize(rect.Width(), rect.Height());

    SubscribeToEvent(E_PROJECTUSERPREFSAVED, ATOMIC_HANDLER(SceneEditor3D, HandleUserPrefSaved));

    SubscribeToEvent(scene_, E_SCENEEDITNODECREATED, ATOMIC_HANDLER(SceneEditor3D, HandleSceneEditNodeCreated));

    SubscribeToEvent(E_EDITORPLAYERSTARTED, ATOMIC_HANDLER(SceneEditor3D, HandlePlayStarted));
    SubscribeToEvent(E_EDITORPLAYERSTOPPED, ATOMIC_HANDLER(SceneEditor3D, HandlePlayStopped));
    SubscribeToEvent(scene_, E_SCENEEDITSCENEMODIFIED, ATOMIC_HANDLER(SceneEditor3D, HandleSceneEditSceneModified));

    SubscribeToEvent(scene_, E_CUBEMAPRENDERBEGIN, ATOMIC_HANDLER(SceneEditor3D, HandleCubemapRenderBegin));
    SubscribeToEvent(scene_, E_CUBEMAPRENDEREND, ATOMIC_HANDLER(SceneEditor3D, HandleCubemapRenderEnd));

    RegisterSceneEditor();

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
            selection_->Delete();
        }

    }

    if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (ev.ref_id == TBIDC("copy"))
        {
            selection_->Copy();
            return true;
        }
        else if (ev.ref_id == TBIDC("paste"))
        {
            selection_->Paste();
            return true;
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

void SceneEditor3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!cubemapRenderCount_)
        gizmo3D_->Update();
}

void SceneEditor3D::HandlePlayStarted(StringHash eventType, VariantMap& eventData)
{

}

void SceneEditor3D::HandlePlayStopped(StringHash eventType, VariantMap& eventData)
{
}

void SceneEditor3D::HandleGizmoEditModeChanged(StringHash eventType, VariantMap& eventData)
{
    EditMode mode = (EditMode) (eventData[GizmoEditModeChanged::P_MODE].GetInt());
    gizmo3D_->SetEditMode(mode);
}

void SceneEditor3D::HandleGizmoAxisModeChanged(StringHash eventType, VariantMap& eventData)
{
    AxisMode mode = (AxisMode) (eventData[GizmoAxisModeChanged::P_MODE].GetInt());
    gizmo3D_->SetAxisMode(mode);
}


void SceneEditor3D::Close(bool navigateToAvailableResource)
{

    VariantMap data;
    data["Scene"] = scene_;
    SendEvent("EditorSceneClosed", data);

    selection_ = 0;
    editHistory_ = 0;

    if (sceneImporter_.NotNull())
    {
        sceneImporter_->SetSceneCamPosition(sceneView_->GetCameraNode()->GetWorldPosition());
        sceneImporter_->SetSceneCamRotation(sceneView_->GetCameraNode()->GetWorldRotation());
        sceneImporter_->GetAsset()->Save();
        sceneImporter_ = nullptr;
    }

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

void SceneEditor3D::RegisterNode(Node * node)
{
    VariantMap eventData;
    eventData[SceneEditAddRemoveNodes::P_END] = false;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

    // generate scene edit event

    VariantMap nodeAddedEventData;
    nodeAddedEventData[SceneEditNodeAdded::P_NODE] = node;
    nodeAddedEventData[SceneEditNodeAdded::P_PARENT] = node->GetParent();
    nodeAddedEventData[SceneEditNodeAdded::P_SCENE] = scene_;
    scene_->SendEvent(E_SCENEEDITNODEADDED, nodeAddedEventData);

    eventData[SceneEditAddRemoveNodes::P_END] = true;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

}

void SceneEditor3D::RegisterNodes(const PODVector<Node*>& nodes)
{
    VariantMap eventData;
    eventData[SceneEditAddRemoveNodes::P_END] = false;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

    // generate scene edit event

    for (unsigned i = 0; i < nodes.Size(); i++)
    {
        Node* node = nodes[i];
        VariantMap nodeAddedEventData;
        nodeAddedEventData[SceneEditNodeAdded::P_NODE] = node;
        nodeAddedEventData[SceneEditNodeAdded::P_PARENT] = node->GetParent();
        nodeAddedEventData[SceneEditNodeAdded::P_SCENE] = scene_;
        scene_->SendEvent(E_SCENEEDITNODEADDED, nodeAddedEventData);
    }

    eventData[SceneEditAddRemoveNodes::P_END] = true;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

}

void SceneEditor3D::Undo()
{
    editHistory_->Undo();
}

void SceneEditor3D::Redo()
{
    editHistory_->Redo();
}

void SceneEditor3D::Cut()
{
    selection_->Cut();
}

void SceneEditor3D::Copy()
{
    selection_->Copy();
}

void SceneEditor3D::Paste()
{
    selection_->Paste();
}

void SceneEditor3D::HandleSceneEditNodeCreated(StringHash eventType, VariantMap& eventData)
{
    PODVector<Node*> nodes;
    nodes.Push(static_cast<Node*>(eventData[SceneEditNodeCreated::P_NODE].GetPtr()));
    RegisterNodes(nodes);
    selection_->AddNode(nodes[0], true);
}

void SceneEditor3D::HandleSceneEditSceneModified(StringHash eventType, VariantMap& eventData)
{
    SetModified(true);
}

void SceneEditor3D::HandleUserPrefSaved(StringHash eventType, VariantMap& eventData)
{
    UpdateGizmoSnapSettings();
}

void SceneEditor3D::UpdateGizmoSnapSettings()
{
    gizmo3D_->SetSnapTranslationX(userPrefs_->GetSnapTranslationX());
    gizmo3D_->SetSnapTranslationY(userPrefs_->GetSnapTranslationY());
    gizmo3D_->SetSnapTranslationZ(userPrefs_->GetSnapTranslationZ());
    gizmo3D_->SetSnapRotation(userPrefs_->GetSnapRotation());
    gizmo3D_->SetSnapScale(userPrefs_->GetSnapScale());

}

void SceneEditor3D::InvokeShortcut(const String& shortcut)
{
    if (shortcut == "frameselected")
    {
        sceneView_->FrameSelection();
        return;
    }

    ResourceEditor::InvokeShortcut(shortcut);
}

void SceneEditor3D::ReparentNode(Node* node, Node* newParent)
{
    // can't parent to self
    if (node == newParent)
        return;

    // already parented
    Node* oldParent = node->GetParent();
    if (oldParent == newParent)
        return;

    // must be in same scene
    if (node->GetScene() != newParent->GetScene())
    {
        return;
    }

    // check if dropping on child of ourselves

    Node* parent = newParent;

    while (parent)
    {
        if (parent == node)
        {
            return;
        }

        parent = parent->GetParent();
    }

    selection_->AddNode(node, true);

    Matrix3x4 transform = node->GetWorldTransform();

    newParent->AddChild(node);

    node->SetWorldTransform(transform.Translation(), transform.Rotation(), transform.Scale());

    scene_->SendEvent(E_SCENEEDITEND);

    PODVector<Node*> nodes;
    node->GetChildren(nodes, true);
    nodes.Insert(0, node);

    VariantMap evData;
    for (unsigned i = 0; i < nodes.Size(); i++)
    {
        evData[SceneEditNodeReparent::P_NODE] = nodes[i];
        evData[SceneEditNodeReparent::P_ADDED] = false;
        scene_->SendEvent(E_SCENEEDITNODEREPARENT, evData);
    }

    evData[SceneEditNodeReparent::P_NODE] = node;
    evData[SceneEditNodeReparent::P_ADDED] = true;
    scene_->SendEvent(E_SCENEEDITNODEREPARENT, evData);

    selection_->AddNode(node, true);


}

void SceneEditor3D::RegisterSceneEditor()
{
    // prune released scene editors
    for (Vector<WeakPtr<SceneEditor3D> >::Iterator i = sceneEditors_.Begin(); i != sceneEditors_.End();)
    {
        if (*i)
        {
            ++i;
        }
        else
        {
            i = sceneEditors_.Erase(i);
        }
    }

    sceneEditors_.Push(WeakPtr<SceneEditor3D>(this));

}

SceneEditor3D* SceneEditor3D::GetSceneEditor(Scene* scene)
{

    for (Vector<WeakPtr<SceneEditor3D> >::Iterator i = sceneEditors_.Begin(); i != sceneEditors_.End();)
    {
        if (*i && scene == (*i)->GetScene())
            return *i;

        i++;
    }

    return NULL;

}

void SceneEditor3D::HandleCubemapRenderBegin(StringHash eventType, VariantMap& eventData)
{
    cubemapRenderCount_++;

    if (cubemapRenderCount_ == 1)
    {
        // first time
        sceneView_->GetDebugRenderer()->SetEnabled(false);
        gizmo3D_->Hide();
    }

}

void SceneEditor3D::HandleCubemapRenderEnd(StringHash eventType, VariantMap& eventData)
{
    cubemapRenderCount_--;

    if (cubemapRenderCount_ == 0)
    {
        // last one
        sceneView_->GetDebugRenderer()->SetEnabled(true);
        gizmo3D_->Show();
    }

}


}
