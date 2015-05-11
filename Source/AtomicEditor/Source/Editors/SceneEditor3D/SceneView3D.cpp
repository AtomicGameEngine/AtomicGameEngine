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
#include <Atomic/Atomic3D/Terrain.h>

#include <Atomic/Input/Input.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include <Atomic/Physics/PhysicsWorld.h>

#include "AEEditor.h"
#include "AEEvents.h"

#include "SceneView3D.h"
#include "SceneEditor3D.h"
#include <Atomic/UI/UI.h>

namespace AtomicEditor
{

SceneView3D ::SceneView3D(Context* context, SceneEditor3D *sceneEditor) :
    UIView3D(context),
    yaw_(0.0f),
    pitch_(0.0f),
    mouseLeftDown_(false),
    mouseMoved_(false),
    enabled_(true)
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
    cameraNode_->SetTemporary(true);
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

    delegate_->SetIsFocusable(true);


}

SceneView3D::~SceneView3D()
{

}

void SceneView3D::Enable()
{
    if (enabled_)
        return;

    enabled_ = true;

    view3DWidget_->SetVisibilility(WIDGET_VISIBILITY_VISIBLE);
}

void SceneView3D::Disable()
{
    if (!enabled_)
        return;

    enabled_ = false;

    view3DWidget_->SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);

}

void SceneView3D::MoveCamera(float timeStep)
{
    if (!enabled_)
        return;

    Input* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.2f;

    if (input->GetKeyDown(KEY_LSHIFT) || input->GetKeyDown(KEY_RSHIFT))
        MOVE_SPEED *= 3.0f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    if (input->GetMouseButtonDown(MOUSEB_RIGHT))
    {
        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);
        // Not working on OSX
        //input->SetMouseMode(MM_RELATIVE);
    }
    else
    {
        // Not working on OSX
        /*
        if (input->GetMouseMode() != MM_ABSOLUTE)
            input->SetMouseMode(MM_ABSOLUTE);
        */
    }


    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    //Vector3 pos = cameraNode_->GetWorldPosition();
    //Quaternion q = cameraNode_->GetWorldRotation();
    //LOGINFOF("%f %f %f : %f %f %f %f", pos.x_, pos.y_, pos.z_, q.x_, q.y_, q.z_, q.w_ );

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

    Input* input = GetSubsystem<Input>();
    IntVector2 cpos = input->GetMousePosition();

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

bool SceneView3D::MouseInView()
{
    Input* input = GetSubsystem<Input>();
    IntVector2 pos = input->GetMousePosition();

    TBRect rect = GetWidgetDelegate()->GetRect();
    GetWidgetDelegate()->ConvertToRoot(rect.x, rect.y);

    return rect.Contains(TBPoint(pos.x_, pos.y_));

}


void SceneView3D::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{

    // Visualize the currently selected nodes
    if (selectedNode_.NotNull())
    {
        DrawNodeDebug(selectedNode_, debugRenderer_);

    }

    if (!MouseInView())
        return;

    Input* input = GetSubsystem<Input>();

    mouseLeftDown_ = false;

    if (input->GetMouseButtonPress(MOUSEB_LEFT))
    {
        if (!mouseMoved_ && !sceneEditor_->GetGizmo()->Selected())
        {
            Ray camRay  = GetCameraRay();
            PODVector<RayQueryResult> result;

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

        mouseMoved_ = false;

    }
    else if (!input->GetMouseButtonDown(MOUSEB_LEFT))
    {

        Ray camRay  = GetCameraRay();
        PODVector<RayQueryResult> result;

        mouseMoved_ = false;

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
    else
    {
        mouseLeftDown_ = true;
        if (Abs(input->GetMouseMoveX() > 3 || input->GetMouseMoveY() >  3))
        {
            mouseMoved_ = true;
        }
    }

}

void SceneView3D::SelectNode(Node* node)
{
    selectedNode_ = node;
}

bool SceneView3D::OnEvent(const TBWidgetEvent &ev)
{
    return sceneEditor_->OnEvent(ev);
}


void SceneView3D::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    // Timestep parameter is same no matter what event is being listened to
    float timeStep = eventData[Update::P_TIMESTEP].GetFloat();

    if (MouseInView())
        MoveCamera(timeStep);

    QueueUpdate();
}

void SceneView3D::HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData)
{
    Node* node = (Node*) (eventData[EditorActiveNodeChange::P_NODE].GetPtr());
    SelectNode(node);
}



}
