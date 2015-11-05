
#include <Atomic/IO/Log.h>
#include <Atomic/Core/Timer.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/Component.h>
#include <Atomic/Scene/SceneEvents.h>

#include "../../EditorMode/AEEditorEvents.h"

#include "SceneEditOps.h"
#include "SceneEditor3DEvents.h"
#include "SceneEditHistory.h"

namespace AtomicEditor
{

SceneEditHistory::SceneEditHistory(Context* context, Scene* scene) :
    Object(context),
    scene_(scene),
    locked_(false)
{
    SubscribeToEvent(E_HISTORYNODEADDED, HANDLER(SceneEditHistory, HandleHistoryNodeAdded));
    SubscribeToEvent(E_HISTORYNODEREMOVED, HANDLER(SceneEditHistory, HandleHistoryNodeRemoved));
    SubscribeToEvent(E_HISTORYNODECHANGED, HANDLER(SceneEditHistory, HandleHistoryNodeChanged));

    SubscribeToEvent(E_HISTORYCOMPONENTCHANGED, HANDLER(SceneEditHistory, HandleHistoryComponentChanged));

    SubscribeToEvent(E_EDITORACTIVENODECHANGE, HANDLER(SceneEditHistory, HandleEditorActiveNodeChange));
}

SceneEditHistory::~SceneEditHistory()
{

}

void SceneEditHistory::AddUndoOp(SceneHistoryOp* op)
{
    undoHistory_.Push(op);

    // adding a new undo op invalids redos
    for (unsigned i = 0; i < redoHistory_.Size(); i++)
    {
        delete redoHistory_[i];
    }

    if (activeNode_.NotNull())
    {
        if (op->type_ == SCENEHISTORYOP_NODECHANGED)
        {
            activeNodeCurrentState_.Clear();
            activeNode_->Animatable::Save(activeNodeCurrentState_);
            activeNodeCurrentState_.Seek(0);
        }

        if (op->type_ == SCENEHISTORYOP_COMPONENTCHANGED)
        {
            ComponentChangedOp* ccop = (ComponentChangedOp*) op;
            currentComponentStates_[ccop->component_].Clear();
            ccop->component_->Animatable::Save(currentComponentStates_[ccop->component_]);
            currentComponentStates_[ccop->component_].Seek(0);
        }
    }

    redoHistory_.Clear();
}

void SceneEditHistory::HandleHistoryNodeAdded(StringHash eventType, VariantMap& eventData)
{
    if (locked_)
        return;

    Scene* scene = static_cast<Scene*>(eventData[HistoryNodeAdded::P_SCENE].GetPtr());
    if (scene != scene_)
        return;

    Node* node = static_cast<Node*>(eventData[HistoryNodeAdded::P_NODE].GetPtr());

    NodeAddedOp* op = new NodeAddedOp(node);
    AddUndoOp(op);
}

void SceneEditHistory::HandleHistoryNodeChanged(StringHash eventType, VariantMap& eventData)
{
    if (locked_)
        return;

    Scene* scene = static_cast<Scene*>(eventData[HistoryNodeChanged::P_SCENE].GetPtr());
    if (scene != scene_)
        return;

    Node* node = static_cast<Node*>(eventData[HistoryNodeChanged::P_NODE].GetPtr());

    NodeChangedOp* op = new NodeChangedOp(node, activeNodeCurrentState_);

    if (activeNodeCurrentState_.GetSize() != op->newState_.GetSize() ||
            memcmp(activeNodeCurrentState_.GetData(), op->newState_.GetData(), op->newState_.GetSize()))
    {
        activeNodeCurrentState_ = op->newState_;
        AddUndoOp(op);
    }
    else
    {
        delete op;
    }

}

void SceneEditHistory::HandleHistoryNodeRemoved(StringHash eventType, VariantMap& eventData)
{
    if (locked_)
        return;
}

void SceneEditHistory::HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData)
{
    Node* node = static_cast<Node*>(eventData[EditorActiveNodeChange::P_NODE].GetPtr());

    if (activeNode_ == node)
        return;

    activeNode_ = node;
    activeNodeCurrentState_.Clear();
    currentComponentStates_.Clear();

    if (activeNode_.Null())
        return;

    node->Animatable::Save(activeNodeCurrentState_);
    activeNodeCurrentState_.Seek(0);

    const Vector<SharedPtr<Component> >& comps = activeNode_->GetComponents();
    for (unsigned i = 0; i < comps.Size(); i++)
    {
        comps[i]->Animatable::Save(currentComponentStates_[comps[i]]);
        currentComponentStates_[comps[i]].Seek(0);
    }
}

void SceneEditHistory::HandleHistoryComponentChanged(StringHash eventType, VariantMap& eventData)
{
    if (locked_)
        return;

    Component* component = static_cast<Component*>(eventData[HistoryComponentChanged::P_COMPONENT].GetPtr());

    if (!component || component->GetNode() != activeNode_)
        return;

    assert(currentComponentStates_.Contains(component));

    ComponentChangedOp* op = new ComponentChangedOp(component, currentComponentStates_[component]);

    if (currentComponentStates_[component].GetSize() != op->newState_.GetSize() ||
            memcmp(currentComponentStates_[component].GetData(), op->newState_.GetData(), op->newState_.GetSize()))
    {
        activeNodeCurrentState_ = op->newState_;
        AddUndoOp(op);
    }
    else
    {
        delete op;
    }


}

void SceneEditHistory::BeginUndoRedo()
{
    //VariantMap evData;
    //evData[SceneUndoRedoBegin::P_SCENE] = scene_;
    //SendEvent(E_SCENEUNDOREDOBEGIN, evData);
}

void SceneEditHistory::EndUndoRedo()
{
    //VariantMap evData;
    //evData[SceneUndoRedoEnd::P_SCENE] = scene_;
    //SendEvent(E_SCENEUNDOREDOEND, evData);
}

void SceneEditHistory::Undo()
{
    if (!undoHistory_.Size())
        return;

    SceneHistoryOp* op = undoHistory_.Back();
    undoHistory_.Pop();

    op->Undo();

    VariantMap eventData;
    if (op->type_ == SCENEHISTORYOP_NODECHANGED)
    {
        NodeChangedOp* ncop = (NodeChangedOp*) op;
        eventData[HistoryNodeChangedUndoRedo::P_NODE] = ncop->node_;
        ncop->node_->SendEvent(E_HISTORYNODECHANGEDUNDOREDO, eventData);
    }
    else if (op->type_ == SCENEHISTORYOP_COMPONENTCHANGED)
    {
        ComponentChangedOp* ccop = (ComponentChangedOp*) op;
        eventData[HistoryComponentChangedUndoRedo::P_COMPONENT] = ccop->component_;
        ccop->component_->SendEvent(E_HISTORYCOMPONENTCHANGEDUNDOREDO, eventData);
    }

    redoHistory_.Push(op);
}

void SceneEditHistory::Redo()
{
    if (!redoHistory_.Size())
        return;

    SceneHistoryOp* op = redoHistory_.Back();
    redoHistory_.Pop();

    op->Redo();

    VariantMap eventData;
    if (op->type_ == SCENEHISTORYOP_NODECHANGED)
    {
        NodeChangedOp* ncop = (NodeChangedOp*) op;
        eventData[HistoryNodeChangedUndoRedo::P_NODE] = ncop->node_;
        ncop->node_->SendEvent(E_HISTORYNODECHANGEDUNDOREDO, eventData);
    }
    else if (op->type_ == SCENEHISTORYOP_COMPONENTCHANGED)
    {
        ComponentChangedOp* ccop = (ComponentChangedOp*) op;
        eventData[HistoryComponentChangedUndoRedo::P_COMPONENT] = ccop->component_;
        ccop->component_->SendEvent(E_HISTORYCOMPONENTCHANGEDUNDOREDO, eventData);
    }

    undoHistory_.Push(op);

}

}
