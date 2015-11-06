
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
    scene_(scene)
{
    SubscribeToEvent(scene_, E_SCENEEDITSERIALIZABLE, HANDLER(SceneEditHistory, HandleSceneEditSerializable));    
    SubscribeToEvent(scene_, E_SCENEEDITNODEADDEDREMOVED, HANDLER(SceneEditHistory, HandleSceneEditNodeAddedRemoved));
}

SceneEditHistory::~SceneEditHistory()
{

}

void SceneEditHistory::AddUndoOp(SceneEditOp* op)
{
    undoHistory_.Push(op);

    scene_->SendEvent(E_SCENEEDITSCENEMODIFIED);

    for (unsigned i = 0; i < redoHistory_.Size(); i++)
    {
        delete redoHistory_[i];
    }

    redoHistory_.Clear();
}

void SceneEditHistory::HandleSceneEditSerializableUndoRedo(StringHash eventType, VariantMap& eventData)
{
    SharedPtr<Serializable> serial(static_cast<Serializable*>(eventData[SceneEditSerializableUndoRedo::P_SERIALIZABLE].GetPtr()));

    if (editStates_.Contains(serial))
    {
        scene_->SendEvent(E_SCENEEDITSCENEMODIFIED);
        editStates_[serial] = eventData[SceneEditSerializableUndoRedo::P_STATE].GetVectorBuffer();
    }
}

void SceneEditHistory::HandleSceneEditNodeAddedRemoved(StringHash eventType, VariantMap& eventData)
{
    bool added = eventData[SceneEditNodeAddedRemoved::P_ADDED].GetBool();
    Node* node = static_cast<Node*>(eventData[SceneEditNodeAddedRemoved::P_NODE].GetPtr());
    NodeAddedRemovedOp* op = new NodeAddedRemovedOp(node, added);
    AddUndoOp(op);
}

void SceneEditHistory::HandleSceneEditSerializable(StringHash eventType, VariantMap& eventData)
{

    int editop = eventData[SceneEditSerializable::P_OPERATION].GetInt();

    SharedPtr<Serializable> serial(static_cast<Serializable*>(eventData[SceneEditSerializable::P_SERIALIZABLE].GetPtr()));

    if (editop == 0) // begin
    {
        if (editStates_.Contains(serial))
            return;

        SubscribeToEvent(serial, E_SCENEEDITSERIALIZABLEUNDOREDO, HANDLER(SceneEditHistory, HandleSceneEditSerializableUndoRedo));
        VectorBuffer& vb = editStates_[serial];
        vb.Clear();
        serial->Serializable::Save(vb);
        vb.Seek(0);
    }
    else if (editop == 1) // change
    {
        if (!editStates_.Contains(serial))
            return;

        VectorBuffer& beginState = editStates_[serial];
        VectorBuffer deltaState;
        serial->Serializable::Save(deltaState);
        deltaState.Seek(0);

        if (beginState.GetSize() != deltaState.GetSize() ||
                memcmp(beginState.GetData(), deltaState.GetData(), deltaState.GetSize()))
        {
            SerializableEditOp* op = new SerializableEditOp(serial, beginState, deltaState);
            AddUndoOp(op);
            editStates_[serial] = deltaState;
        }
    }
    else if (editop == 2) // end
    {
        if (!editStates_.Contains(serial))
            return;

        UnsubscribeFromEvent(serial, E_SCENEEDITSERIALIZABLEUNDOREDO);

        editStates_.Erase(serial);
    }
}

void SceneEditHistory::Undo()
{
    if (!undoHistory_.Size())
        return;

    SceneEditOp* op = undoHistory_.Back();
    undoHistory_.Pop();

    op->Undo();

    redoHistory_.Push(op);

}

void SceneEditHistory::Redo()
{
    if (!redoHistory_.Size())
        return;

    SceneEditOp* op = redoHistory_.Back();
    redoHistory_.Pop();

    op->Redo();

    undoHistory_.Push(op);

}

}
