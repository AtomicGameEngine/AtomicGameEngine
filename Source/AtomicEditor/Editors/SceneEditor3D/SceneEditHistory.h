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
class Node;
}

namespace AtomicEditor
{

class SceneEditor3D;
class SelectionEditOp;

class SceneEditHistory: public Object
{
    OBJECT(SceneEditHistory);

public:

    SceneEditHistory(Context* context, SceneEditor3D* sceneEditor);
    virtual ~SceneEditHistory();

    void BeginSelectionEdit();
    void EndSelectionEdit(bool begin = true);

    /// Removes a node from the edit history
    void RemoveNode(Node* node);

    void Undo();
    void Redo();

private:

    void HandleSceneNodeSelected(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditBegin(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditEnd(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditAddRemoveNodes(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditNodeAdded(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditNodeRemoved(StringHash eventType, VariantMap& eventData);

    void AddUndoOp(SelectionEditOp* op);

    SharedPtr<SceneEditor3D> sceneEditor_;

    SelectionEditOp* curSelEditOp_;

    PODVector<SelectionEditOp*> undoHistory_;
    PODVector<SelectionEditOp*> redoHistory_;

    bool addingRemovingNodes_;

};

}

