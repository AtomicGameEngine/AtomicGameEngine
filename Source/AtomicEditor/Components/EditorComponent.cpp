//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/Context.h>

#include <Atomic/Graphics/Camera.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Graphics/Texture2D.h>

#include "../Editors/SceneEditor3D/SceneEditor3D.h"

#include "EditorComponent.h"

namespace AtomicEditor
{

EditorComponent::EditorComponent(Context *context) : Component(context)
{

}

EditorComponent::~EditorComponent()
{

}

void EditorComponent::RegisterObject(Context* context)
{
    context->RegisterFactory<EditorComponent>();
}

SceneEditor3D* EditorComponent::GetSceneEditor()
{
    if (!GetScene())
        return NULL;

    return SceneEditor3D::GetSceneEditor(GetScene());

}

}
