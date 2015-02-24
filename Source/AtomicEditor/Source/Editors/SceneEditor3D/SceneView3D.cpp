// Portions Copyright (c) 2008-2015 the Urho3D project.

// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/Log.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Graphics/Camera.h>

#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/DebugRenderer.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Graphics/Octree.h>
#include <Atomic/Graphics/Terrain.h>

#include <Atomic/Input/Input.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include <Atomic/Physics/PhysicsWorld.h>

#include "AEEditor.h"
#include "AEEvents.h"

#include "SceneView3D.h"
#include "SceneEditor3D.h"

#include <Atomic/UI/UI.h>
#include <Atomic/UI/TBUI.h>

namespace AtomicEditor
{

SceneView3D ::SceneView3D(Context* context, SceneEditor3D *sceneEditor) :
    View3D(context),
    yaw_(0.0f),
    pitch_(0.0f)
{

    sceneEditor_ = sceneEditor;

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = sceneEditor->GetScene();

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

    cameraNode_ = scene_->CreateChild("Camera");
    camera_ = cameraNode_->CreateComponent<Camera>();

    debugRenderer_ = scene_->GetComponent<DebugRenderer>();
    assert(debugRenderer_.NotNull());
    octree_ = scene_->GetComponent<Octree>();
    assert(octree_.NotNull());

    cameraNode_->SetPosition(Vector3(0, 0, -10));

    SetView(scene_, camera_);
    SetAutoUpdate(false);

    SubscribeToEvent(E_UPDATE, HANDLER(SceneView3D, HandleUpdate));
    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(SceneView3D, HandleEditorActiveNodeChange));
    SubscribeToEvent(E_POSTRENDERUPDATE, HANDLER(SceneView3D, HandlePostRenderUpdate));

    // TODO: generate this event properly
    VariantMap eventData;
    eventData[EditorActiveSceneChange::P_SCENE] = scene_;
    SendEvent(E_EDITORACTIVESCENECHANGE, eventData);

}

SceneView3D::~SceneView3D()
{

}

void SceneView3D::MoveCamera(float timeStep)
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

Ray SceneView3D::GetCameraRay()
{
    Ray camRay;

    UI* ui = GetSubsystem<UI>();

    IntVector2 cpos = ui->GetCursorPosition();

    TBRect rect = GetWidgetDelegate()->GetRect();

    if (!rect.w || !rect.h)
        return camRay;

    GetWidgetDelegate()->ConvertToRoot(rect.x, rect.y);

    return  camera_->GetScreenRay(float(cpos.x_ - rect.x) / rect.w,
                                       float(cpos.y_ - rect.y) /rect.h);
}

void SceneView3D::DrawNodeDebug(Node* node, DebugRenderer* debug, bool drawNode)
{
    if (drawNode)
        debug->AddNode(node, 1.0, false);

    // Exception for the scene to avoid bringing the editor to its knees: drawing either the whole hierarchy or the subsystem-
    // components can have a large performance hit. Also do not draw terrain child nodes due to their large amount
    // (TerrainPatch component itself draws nothing as debug geometry)
    if (node != scene_ && !node->GetComponent<Terrain>())
    {
        const Vector<SharedPtr<Component> >& components = node->GetComponents();

        for (unsigned j = 0; j < components.Size(); ++j)
            components[j]->DrawDebugGeometry(debug, false);

        // To avoid cluttering the view, do not draw the node axes for child nodes
        for (unsigned k = 0; k < node->GetNumChildren(); ++k)
            DrawNodeDebug(node->GetChild(k), debug, false);
    }
}


void SceneView3D::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{

    // Visualize the currently selected nodes
    if (selectedNode_.NotNull())
    {
        DrawNodeDebug(selectedNode_, debugRenderer_);

    }

    Ray camRay  = GetCameraRay();
    PODVector<RayQueryResult> result;

    /*
    Array<int> pickModeDrawableFlags = {
        DRAWABLE_GEOMETRY,
        DRAWABLE_LIGHT,
        DRAWABLE_ZONE
    };
    */

    RayOctreeQuery query(result, camRay, RAY_TRIANGLE, camera_->GetFarClip(), DRAWABLE_GEOMETRY, 0x7fffffff);
    octree_->RaycastSingle(query);

    if (query.result_.Size())
    {
        const RayQueryResult& r = result[0];

        if (r.drawable_)
        {
            debugRenderer_->AddNode(r.drawable_->GetNode(), 1.0, false);
            r.drawable_->DrawDebugGeometry(debugRenderer_, false);
        }

    }

}

void SceneView3D::SelectNode(Node* node)
{
    selectedNode_ = node;
}

bool SceneView3D::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK)
    {
        Ray camRay  = GetCameraRay();
        PODVector<RayQueryResult> result;

        /*
        Array<int> pickModeDrawableFlags = {
            DRAWABLE_GEOMETRY,
            DRAWABLE_LIGHT,
            DRAWABLE_ZONE
        };
        */
        RayOctreeQuery query(result, camRay, RAY_TRIANGLE, camera_->GetFarClip(), DRAWABLE_GEOMETRY, 0x7fffffff);
        octree_->RaycastSingle(query);

        if (query.result_.Size())
        {
            const RayQueryResult& r = result[0];

            if (r.drawable_)
            {

                VariantMap neventData;
                neventData[EditorActiveNodeChange::P_NODE] = r.drawable_->GetNode();
                SendEvent(E_EDITORACTIVENODECHANGE, neventData);

            }
        }
    }

    return false;
}


void SceneView3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    // Timestep parameter is same no matter what event is being listened to
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    MoveCamera(timeStep);

    QueueUpdate();
}

void SceneView3D::HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData)
{
    Node* node = (Node*) (eventData[EditorActiveNodeChange::P_NODE].GetPtr());
    SelectNode(node);
}



}
