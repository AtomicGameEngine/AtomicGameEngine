// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <TurboBadger/tb_widgets_common.h>
#include "ResourceEditor.h"

using namespace Atomic;
using namespace tb;

namespace Atomic
{
class Scene;
class Node;
class View3D;
class Camera;
class DebugRenderer;
class Octree;
}

namespace AtomicEditor
{

class JSAutocomplete;

class SceneResourceEditor: public ResourceEditor
{
    OBJECT(SceneResourceEditor);

public:

    SceneResourceEditor(Context* context, const String& fullpath, TBTabContainer* container);

    virtual ~SceneResourceEditor();

    bool OnEvent(const TBWidgetEvent &ev);

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);

    void MoveCamera(float timeStep);

    SharedPtr<Scene> scene_;
    SharedPtr<Node> cameraNode_;
    SharedPtr<View3D> view3D_;
    SharedPtr<Camera> camera_;
    float yaw_;
    float pitch_;

    TBLayout* layout_;
    TBContainer* view3DContainer_;

    WeakPtr<DebugRenderer> debugRenderer_;
    WeakPtr<Octree> octree_;

};

}
