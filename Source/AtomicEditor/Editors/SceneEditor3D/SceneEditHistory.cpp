//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/Scene/SceneEvents.h>

#include "SceneEditor3D.h"

#include "SceneEditor3DEvents.h"
#include "SceneEditOp.h"
#include "SceneSelection.h"
#include "SceneEditHistory.h"

namespace AtomicEditor
{

SceneEditHistory::SceneEditHistory(Context* context, SceneEditor3D* sceneEditor) : Object(context),
    sceneEditor_(sceneEditor),
    curSelEditOp_(0),
    addingRemovingNodes_(false)
{
    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENENODESELECTED, HANDLER(SceneEditHistory, HandleSceneNodeSelected));

    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENEEDITBEGIN, HANDLER(SceneEditHistory, HandleSceneEditBegin));
    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENEEDITEND, HANDLER(SceneEditHistory, HandleSceneEditEnd));

    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENEEDITADDREMOVENODES, HANDLER(SceneEditHistory, HandleSceneEditAddRemoveNodes));

    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENEEDITNODEADDED, HANDLER(SceneEditHistory, HandleSceneEditNodeAdded));
    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENEEDITNODEREMOVED, HANDLER(SceneEditHistory, HandleSceneEditNodeRemoved));

}

SceneEditHistory::~SceneEditHistory()
{

}

void SceneEditHistory::HandleSceneEditBegin(StringHash eventType, VariantMap& eventData)
{
    assert(0);
    BeginSelectionEdit();
}

void SceneEditHistory::HandleSceneEditEnd(StringHash eventType, VariantMap& eventData)
{
    EndSelectionEdit();
}

void SceneEditHistory::HandleSceneEditAddRemoveNodes(StringHash eventType, VariantMap& eventData)
{

    bool end = eventData[SceneEditAddRemoveNodes::P_END].GetBool();

    if (end)
    {
        addingRemovingNodes_ = false;

        EndSelectionEdit(true);
    }
    else
    {
        addingRemovingNodes_ = true;
        EndSelectionEdit(false);

        curSelEditOp_ = new SelectionEditOp(sceneEditor_->GetScene());

    }
}

void SceneEditHistory::HandleSceneEditNodeAdded(StringHash eventType, VariantMap& eventData)
{
    if (!addingRemovingNodes_)
        return;

    assert(curSelEditOp_);

    Node* node = static_cast<Node*>(eventData[NodeAdded::P_NODE].GetPtr());
    Node* parent = static_cast<Node*>(eventData[NodeAdded::P_PARENT].GetPtr());

    curSelEditOp_->NodeAdded(node, parent);
}

void SceneEditHistory::HandleSceneEditNodeRemoved(StringHash eventType, VariantMap& eventData)
{
    if (!addingRemovingNodes_)
        return;

    assert(curSelEditOp_);

    Node* node = static_cast<Node*>(eventData[NodeAdded::P_NODE].GetPtr());
    Node* parent = static_cast<Node*>(eventData[NodeAdded::P_PARENT].GetPtr());

    curSelEditOp_->NodeRemoved(node, parent);
}

void SceneEditHistory::AddUndoOp(SelectionEditOp* op)
{
    undoHistory_.Push(op);

    sceneEditor_->GetScene()->SendEvent(E_SCENEEDITSCENEMODIFIED);

    for (unsigned i = 0; i < redoHistory_.Size(); i++)
    {
        delete redoHistory_[i];
    }

    redoHistory_.Clear();
}

void SceneEditHistory::BeginSelectionEdit()
{
    assert(!curSelEditOp_);

    Vector<SharedPtr<Node>>& nodes = sceneEditor_->GetSelection()->GetNodes();
    if (!nodes.Size())
        return;

    curSelEditOp_ = new SelectionEditOp(sceneEditor_->GetScene());
    curSelEditOp_->SetNodes(nodes);
}

void SceneEditHistory::EndSelectionEdit(bool begin)
{
    if (!curSelEditOp_)
        return;

    curSelEditOp_->RegisterEdit();

    if (curSelEditOp_->Commit())
    {
        AddUndoOp(curSelEditOp_);
    }
    else
    {
        delete curSelEditOp_;
    }

    curSelEditOp_ = 0;

    if (begin)
        BeginSelectionEdit();

}

void SceneEditHistory::Undo()
{
    if (!undoHistory_.Size())
        return;

    SelectionEditOp* op = undoHistory_.Back();
    undoHistory_.Pop();

    op->Undo();
    sceneEditor_->GetScene()->SendEvent(E_SCENEEDITSCENEMODIFIED);

    redoHistory_.Push(op);

    if (curSelEditOp_)
    {
        delete curSelEditOp_;
        curSelEditOp_ = 0;
    }

    BeginSelectionEdit();

}

void SceneEditHistory::Redo()
{
    if (!redoHistory_.Size())
        return;

    SelectionEditOp* op = redoHistory_.Back();
    redoHistory_.Pop();

    op->Redo();
    sceneEditor_->GetScene()->SendEvent(E_SCENEEDITSCENEMODIFIED);

    undoHistory_.Push(op);

    if (curSelEditOp_)
    {
        delete curSelEditOp_;
        curSelEditOp_ = 0;
    }

    BeginSelectionEdit();


}


void SceneEditHistory::HandleSceneNodeSelected(StringHash eventType, VariantMap& eventData)
{

    if (eventData[SceneNodeSelected::P_QUIET].GetBool())
        return;

    if (curSelEditOp_)
    {
        EndSelectionEdit();
    }
    else
        BeginSelectionEdit();
}

void SceneEditHistory::RemoveNode(Node *node)
{
    PODVector<SelectionEditOp*> remove;

    for (unsigned i = 0; i < undoHistory_.Size(); i++)
    {

        SelectionEditOp* op = undoHistory_[i];

        if (op->EraseNode(node))
        {
            assert(!remove.Contains(op));
            remove.Push(op);
        }

    }

    for (unsigned i = 0; i < redoHistory_.Size(); i++)
    {

        SelectionEditOp* op = redoHistory_[i];

        if (op->EraseNode(node))
        {
            assert(!remove.Contains(op));
            remove.Push(op);
        }

    }

    for (unsigned i = 0; i < remove.Size(); i++)
    {
        PODVector<SelectionEditOp*>::Iterator itr = undoHistory_.Find(remove[i]);

        if (itr != undoHistory_.End())
        {
            delete *itr;
            undoHistory_.Erase(itr);
        }
        else
        {
            itr = redoHistory_.Find(remove[i]);
            delete *itr;
            redoHistory_.Erase(itr);
        }
    }


}

}

