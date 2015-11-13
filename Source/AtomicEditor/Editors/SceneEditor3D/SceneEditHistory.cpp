
#include "SceneEditor3D.h"

#include "SceneEditor3DEvents.h"
#include "SceneEditOp.h"
#include "SceneSelection.h"
#include "SceneEditHistory.h"

namespace AtomicEditor
{

SceneEditHistory::SceneEditHistory(Context* context, SceneEditor3D* sceneEditor) : Object(context),
    sceneEditor_(sceneEditor),
    curSelEditOp_(0)
{
    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENENODESELECTED, HANDLER(SceneEditHistory, HandleSceneNodeSelected));

    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENEEDITBEGIN, HANDLER(SceneEditHistory, HandleSceneEditBegin));
    SubscribeToEvent(sceneEditor_->GetScene(), E_SCENEEDITEND, HANDLER(SceneEditHistory, HandleSceneEditEnd));

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

void SceneEditHistory::BeginSelectionEdit()
{
    assert(!curSelEditOp_);

    Vector<SharedPtr<Node>>& nodes = sceneEditor_->GetSelection()->GetNodes();
    if (!nodes.Size())
        return;

    curSelEditOp_ = new SelectionEditOp(nodes);
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

void SceneEditHistory::EndSelectionEdit()
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

    BeginSelectionEdit();

}

void SceneEditHistory::Undo()
{
    if (!undoHistory_.Size())
        return;

    SelectionEditOp* op = undoHistory_.Back();
    undoHistory_.Pop();

    op->Undo();

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
    if (curSelEditOp_)
    {
        EndSelectionEdit();
    }
    else
        BeginSelectionEdit();
}


}

