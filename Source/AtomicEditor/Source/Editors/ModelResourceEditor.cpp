// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Graphics/Octree.h>
#include <Atomic/Graphics/Camera.h>
#include <Atomic/Graphics/Zone.h>
#include <Atomic/Atomic3D/AnimatedModel.h>
#include <Atomic/Atomic3D/Animation.h>
#include <Atomic/Atomic3D/AnimationState.h>
#include <Atomic/Atomic3D/AnimationController.h>
#include <Atomic/Graphics/Material.h>
#include <Atomic/Graphics/DebugRenderer.h>

#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/Input/Input.h>

#include "../AEEditor.h"


#include "ModelResourceEditor.h"

#include <Atomic/UI/TBUI.h>
#include <Atomic/UI/UI.h>
#include <Atomic/UI/View3D.h>

namespace AtomicEditor
{

ModelResourceEditor ::ModelResourceEditor(Context* context, const String &fullpath, TBTabContainer *container) :
    ResourceEditor(context, fullpath, container),
    layout_(0),
    view3DContainer_(0),
    yaw_(0.0f),
    pitch_(0.0f)
{
    layout_ = new TBLayout();
    layout_->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    layout_->SetLayoutSize(LAYOUT_SIZE_AVAILABLE);
    layout_->SetSize(container_->GetRect().w, container_->GetRect().h);

    view3DContainer_ = new TBContainer();
    view3DContainer_->SetGravity(WIDGET_GRAVITY_ALL);

    TBLayout* verticalLayout = new TBLayout();
    verticalLayout->SetAxis(AXIS_Y);
    verticalLayout->SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);

    TBButton* button = new TBButton();
    button->SetText("Idle");
    button->SetID(TBIDC("Idle"));
    button->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
    verticalLayout->AddChild(button);
    button = new TBButton();
    button->SetText("Walk");
    button->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
    button->SetID(TBIDC("Walk"));
    verticalLayout->AddChild(button);
    button = new TBButton();
    button->SetText("Run");
    button->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
    button->SetID(TBIDC("Run"));
    verticalLayout->AddChild(button);
    button = new TBButton();
    button->SetText("Turn Left");
    button->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
    button->SetID(TBIDC("TurnLeft"));
    verticalLayout->AddChild(button);
    button = new TBButton();
    button->SetText("Turn Right");
    button->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
    button->SetID(TBIDC("TurnRight"));
    verticalLayout->AddChild(button);
    button = new TBButton();
    button->SetText("Walk Backwards");
    button->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
    button->SetID(TBIDC("WalkBackwards"));
    verticalLayout->AddChild(button);
    button = new TBButton();
    button->SetText("Attack");
    button->SetGravity(WIDGET_GRAVITY_LEFT_RIGHT);
    button->SetID(TBIDC("Attack"));
    verticalLayout->AddChild(button);

    layout_->AddChild(view3DContainer_);
    layout_->AddChild(verticalLayout);

    container_->GetContentRoot()->AddChild(layout_);

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-10000.0f, 10000.f));
    zone->SetAmbientColor(Color(1, 1, 1));

    cameraNode_ = scene_->CreateChild("Camera");
    camera_ = cameraNode_->CreateComponent<Camera>();

    cameraNode_->SetPosition(Vector3(0.0f, .9f, -2.75f));

    // Create a smaller box at camera position
    modelNode_ = scene_->CreateChild("ModelNode");

    AnimatedModel* amodel = modelNode_->CreateComponent<AnimatedModel>();
    Model* model = cache->GetResource<Model>("Models/AS_FatZombie_FBX_FatZombie_LOD0.mdl");
    amodel->SetModel(model);
    amodel->SetMaterial(cache->GetResource<Material>("Materials/AS_FatZombie_Base.xml"));
    amodel->SetCastShadows(true);

    AnimationController* controller = modelNode_->CreateComponent<AnimationController>();
    controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Idle.ani", 0, true);

    modelNode_->Yaw(180);

    view3D_ = new View3D(context_);
    view3D_->SetView(scene_, camera_);
    view3D_->SetAutoUpdate(false);

    GetSubsystem<TBUI>()->AddChild(view3D_);

    SubscribeToEvent(E_UPDATE, HANDLER(ModelResourceEditor, HandleUpdate));

}

ModelResourceEditor::~ModelResourceEditor()
{

}

bool ModelResourceEditor::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        AnimationController* controller = modelNode_->GetComponent<AnimationController>();

        if (ev.target->GetID() == TBIDC("Walk"))
        {
            controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Walk.ani", 0.0f, true);
        }
        else if (ev.target->GetID() == TBIDC("Run"))
        {
            controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Run.ani", 0.0f, true);
        }
        else if (ev.target->GetID() == TBIDC("Idle"))
        {
            controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Idle.ani", 0.0f, true);
        }
        else if (ev.target->GetID() == TBIDC("TurnLeft"))
        {
            controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Turn_Left.ani", 0.0f, true);
        }
        else if (ev.target->GetID() == TBIDC("TurnRight"))
        {
            controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Turn_Right.ani", 0.0f, true);
        }
        else if (ev.target->GetID() == TBIDC("WalkBackwards"))
        {
            controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Walk_Backwards.ani", 0.0f, true);
        }
        else if (ev.target->GetID() == TBIDC("Attack"))
        {
            controller->PlayExclusive("Models/AS_FatZombie_FBX_FatZombie_LOD0_Attack_Melee.ani", 0.0f, true);
        }

    }
    return false;
}

void ModelResourceEditor::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 1.5f;
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
    if (input->GetKeyDown('Q'))
        cameraNode_->Translate(Vector3::UP * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('E'))
        cameraNode_->Translate(Vector3::DOWN * MOVE_SPEED * timeStep);
    if (input->GetKeyDown('A'))
        modelNode_->Yaw(-timeStep * 100);
    if (input->GetKeyDown('D'))
        modelNode_->Yaw(timeStep * 100);
}

void ModelResourceEditor::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    if ((layout_->GetVisibility() != WIDGET_VISIBILITY_VISIBLE) || GetSubsystem<Editor>()->IsPlayingProject())
    {
        if (view3D_->IsVisible())
        {
            view3D_->SetVisible(false);            
        }

        return;
    }

    if (!view3D_->IsVisible())
        view3D_->SetVisible(true);

    // Timestep parameter is same no matter what event is being listened to
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    MoveCamera(timeStep);

    bool dirty = false;

    TBRect rect = view3DContainer_->GetRect();

    TBWidget* parent = view3DContainer_->GetParent();
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

    const IntVector2& size = view3D_->GetScreenPosition();

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
