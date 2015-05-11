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

#include <Atomic/UI/UI.h>

namespace AtomicEditor
{

ModelResourceEditor ::ModelResourceEditor(Context* context, const String &fullpath, TBTabContainer *container) :
    ResourceEditor(context, fullpath, container),
    layout_(0),
    view3DContainer_(0),
    yaw_(0.0f),
    pitch_(0.0f)
{
}

ModelResourceEditor::~ModelResourceEditor()
{

}

bool ModelResourceEditor::OnEvent(const TBWidgetEvent &ev)
{

    return false;
}

void ModelResourceEditor::MoveCamera(float timeStep)
{

}

void ModelResourceEditor::HandleUpdate(StringHash eventType, VariantMap& eventData)
{



}


}
