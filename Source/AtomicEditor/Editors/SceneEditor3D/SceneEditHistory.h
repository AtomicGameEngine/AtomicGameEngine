//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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
    ATOMIC_OBJECT(SceneEditHistory, Object);

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

    WeakPtr<SceneEditor3D> sceneEditor_;

    SelectionEditOp* curSelEditOp_;

    PODVector<SelectionEditOp*> undoHistory_;
    PODVector<SelectionEditOp*> redoHistory_;

    bool addingRemovingNodes_;

};

}
