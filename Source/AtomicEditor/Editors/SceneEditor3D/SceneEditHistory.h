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

class Scene;
class Node;
class Component;

}

namespace AtomicEditor
{

class SceneHistoryOp;

/// Simple scene history to support undo/redo via snapshots of scene state
class SceneEditHistory: public Object
{
    OBJECT(SceneEditHistory);

public:

    SceneEditHistory(Context* context, Scene* scene);
    virtual ~SceneEditHistory();

    void Undo();
    void Redo();

    void Lock() { locked_ = true; }
    void Unlock() { locked_ = false; }

private:

    void HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData);

    void HandleHistoryNodeAdded(StringHash eventType, VariantMap& eventData);
    void HandleHistoryNodeRemoved(StringHash eventType, VariantMap& eventData);
    void HandleHistoryNodeChanged(StringHash eventType, VariantMap& eventData);

    void HandleHistoryComponentChanged(StringHash eventType, VariantMap& eventData);


    void AddUndoOp(SceneHistoryOp* op);

    void BeginUndoRedo();
    void EndUndoRedo();

    WeakPtr<Scene> scene_;

    WeakPtr<Node> activeNode_;
    VectorBuffer activeNodeCurrentState_;

    HashMap<Component*, VectorBuffer> currentComponentStates_;

    bool locked_;
    PODVector<SceneHistoryOp*> undoHistory_;
    PODVector<SceneHistoryOp*> redoHistory_;

};

}
