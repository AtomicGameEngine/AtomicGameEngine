//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;
namespace Atomic
{

class Serializable;
class Scene;

}

namespace AtomicEditor
{

class SceneEditOp;

/// Simple scene history to support undo/redo via snapshots of scene state
class SceneEditHistory: public Object
{
    OBJECT(SceneEditHistory);

public:

    SceneEditHistory(Context* context, Scene* scene);
    virtual ~SceneEditHistory();

    void Undo();
    void Redo();

private:

    void HandleSceneEditSerializable(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditSerializableUndoRedo(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditNodeAddedRemoved(StringHash eventType, VariantMap& eventData);

    void AddUndoOp(SceneEditOp* op);

    WeakPtr<Scene> scene_;

    HashMap< SharedPtr<Serializable>, VectorBuffer > editStates_;

    PODVector<SceneEditOp*> undoHistory_;
    PODVector<SceneEditOp*> redoHistory_;

};

}
