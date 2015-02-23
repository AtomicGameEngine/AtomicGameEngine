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


#include <Atomic/UI/TBUI.h>
#include <Atomic/UI/UI.h>
#include <Atomic/UI/View3D.h>

#include "SceneEditor3D.h"
#include "View3DWidget.h"

namespace AtomicEditor
{

SceneEditor3D ::SceneEditor3D(Context* context, const String &fullpath, TBTabContainer *container) :
    ResourceEditor(context, fullpath, container),
    layout_(0),
    view3DContainer_(0)
{
    layout_ = new TBLayout();
    layout_->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    layout_->SetSize(container_->GetRect().w, container_->GetRect().h);

    view3DContainer_ = new TBContainer();
    view3DContainer_->SetGravity(WIDGET_GRAVITY_ALL);

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    SharedPtr<File> xmlFile = cache->GetFile(fullpath);

    if (GetExtension(fullpath) == ".scene")
        scene_->LoadXML(*xmlFile);
    else
        scene_->Load(*xmlFile);

    scene_->SetUpdateEnabled(false);

    debugRenderer_ = scene_->GetComponent<DebugRenderer>();

    if (debugRenderer_.Null())
    {
        debugRenderer_ = scene_->CreateComponent<DebugRenderer>();
    }

    octree_ = scene_->GetComponent<Octree>();

    if (octree_.Null())
    {
        LOGWARNING("Scene without an octree loaded");
        octree_ = scene_->CreateComponent<Octree>();
    }

    // once octree/debugrenderer exist
    sceneView_ = new SceneView3D(context_, this);

    View3DWidget* widget = sceneView_->GetWidget();
    widget->SetGravity(WIDGET_GRAVITY_ALL);

    view3DContainer_->AddChild(widget);

    layout_->AddChild(view3DContainer_);

    container_->GetContentRoot()->AddChild(layout_);

    SubscribeToEvent(E_UPDATE, HANDLER(SceneEditor3D, HandleUpdate));
    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(SceneEditor3D, HandleEditorActiveNodeChange));

    // TODO: generate this event properly
    VariantMap eventData;
    eventData[EditorActiveSceneChange::P_SCENE] = scene_;
    SendEvent(E_EDITORACTIVESCENECHANGE, eventData);

}

SceneEditor3D::~SceneEditor3D()
{
}

bool SceneEditor3D::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

void SceneEditor3D::SelectNode(Node* node)
{

}

void SceneEditor3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    // view3D_->QueueUpdate();
}

void SceneEditor3D::HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData)
{
    Node* node = (Node*) (eventData[EditorActiveNodeChange::P_NODE].GetPtr());
    SelectNode(node);
}



}
