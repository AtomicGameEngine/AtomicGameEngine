// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/Log.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Graphics/Camera.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include <Atomic/Physics/PhysicsWorld.h>

#include "../AEEditor.h"

#include <Atomic/Input/Input.h>
#include "SceneResourceEditor.h"

#include <Atomic/UI/TBUI.h>
#include <Atomic/UI/UI.h>
#include <Atomic/UI/View3D.h>

namespace AtomicEditor
{

SceneResourceEditor ::SceneResourceEditor(Context* context, const String &fullpath, TBTabContainer *container) :
    ResourceEditor(context, fullpath, container),
    layout_(0),
    view3DContainer_(0),
    yaw_(0.0f),
    pitch_(0.0f)
{
    layout_ = new TBLayout();
    layout_->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    layout_->SetSize(container_->GetRect().w, container_->GetRect().h);

    view3DContainer_ = new TBContainer();
    view3DContainer_->SetGravity(WIDGET_GRAVITY_ALL);

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    SharedPtr<File> xmlFile = cache->GetFile(fullpath);

    if (GetExtension(fullpath) == ".xml")
        scene_->LoadXML(*xmlFile);
    else
        scene_->Load(*xmlFile);

    // TODO: Fix this properly, currently simulates immediately
    PhysicsWorld* physics = scene_->GetComponent<PhysicsWorld>();
    if (physics)
        scene_->RemoveComponent(physics);

    cameraNode_ = scene_->CreateChild("Camera");
    camera_ = cameraNode_->CreateComponent<Camera>();

    if (fullpath_.Contains("CompartmentScene"))
    {
        pitch_ = 19.4f;
        yaw_ = -228.6f;
        cameraNode_->SetPosition(Vector3(-42.66f, 6.68f, 9.34f));
    }
    else
    {
        pitch_ = 26.8f;
        yaw_ = 121.4f;
        cameraNode_->SetPosition(Vector3(-135.81f, 76.56f, 16.42f));
    }


    // in a view3D_ procsky renders wrong (inversed?)
    //Node* skyNode = scene_->CreateChild("ProcSky", LOCAL );
    //skyNode->CreateComponent<ProcSky>();

    layout_->AddChild(view3DContainer_);

    container_->GetContentRoot()->AddChild(layout_);

    view3D_ = new View3D(context_);
    view3D_->SetView(scene_, camera_);
    view3D_->SetAutoUpdate(false);

    GetSubsystem<TBUI>()->AddChild(view3D_);

    SubscribeToEvent(E_UPDATE, HANDLER(SceneResourceEditor, HandleUpdate));

}

SceneResourceEditor::~SceneResourceEditor()
{

}

bool SceneResourceEditor::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

void SceneResourceEditor::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.2f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    if (input->GetKeyDown('F'))
    {
        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);
    }

    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    // Use the Translate() function (default local space) to move relative to the node's orientation.
    if (input->GetKeyDown('W'))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('S'))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('A'))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('D'))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

void SceneResourceEditor::HandleUpdate(StringHash eventType, VariantMap& eventData)
{    

    if ((layout_->GetVisibility() != WIDGET_VISIBILITY_VISIBLE) || GetSubsystem<Editor>()->IsPlayingProject())
    {
        if (view3D_->IsVisible())
            view3D_->SetVisible(false);

        return;
    }

    if (!view3D_->IsVisible())
        view3D_->SetVisible(true);

    /*
    Vector3 cpos = cameraNode_->GetPosition();
    Quaternion crot = cameraNode_->GetRotation();
    LOGINFOF("%f %f %f - %f %f", cpos.x_, cpos.y_, cpos.z_, pitch_, yaw_);
    */

    // Timestep parameter is same no matter what event is being listened to
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    MoveCamera(timeStep);

    bool dirty = false;

    TBRect rect = layout_->GetRect();
    TBWidget* parent = layout_->GetParent();
    while (parent)
    {
        TBRect prect = parent->GetRect();
        rect.x += prect.x;
        rect.y += prect.y;
        parent = parent->GetParent();
    }

    const IntVector2& pos = view3D_->GetPosition();
    if (pos.x_ != rect.x || pos.y_ != rect.y)
        dirty = true;

    const IntVector2& size = view3D_->GetSize();

    if (size.x_ != rect.w || size.y_ != rect.h)
        dirty = true;

    if (dirty)
    {
        view3D_->SetPosition(rect.x, rect.y);
        view3D_->SetWidth(rect.w);
        view3D_->SetHeight(rect.h);
    }

    view3D_->QueueUpdate();
}


}
