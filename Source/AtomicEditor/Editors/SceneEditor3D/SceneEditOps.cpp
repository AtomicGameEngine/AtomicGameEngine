
#include <Atomic/IO/Log.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Component.h>

#include "SceneEditOps.h"
#include "SceneEditor3DEvents.h"

namespace AtomicEditor
{


NodeEditOp::NodeEditOp(Node* node)
{
    node_ = node;

    // parent at the time of the operation
    parent_ = node->GetParent();
}

NodeAddedRemovedOp::NodeAddedRemovedOp(Node* node, bool added) : NodeEditOp(node),
    added_(added)
{
    type_ = SCENEEDIT_NODE_ADDED_REMOVED;
}

bool NodeAddedRemovedOp::Undo()
{
    if (added_)
    {
        node_->Remove();
    }
    else
    {
        parent_->AddChild(node_);
    }

    return true;
}

bool NodeAddedRemovedOp::Redo()
{
    if (added_)
    {
        parent_->AddChild(node_);
    }
    else
    {
        node_->Remove();
    }

    return true;
}

SerializableEditOp::SerializableEditOp(Serializable* serializable, const VectorBuffer& beginState, const VectorBuffer& endState) : SceneEditOp(),
    serializable_(serializable),
    beginState_(beginState),
    endState_(endState)
{
    type_ = SCENEEDIT_SERIALIZABLE_EDIT;

    if (!beginState_.GetSize())
    {
        LOGERRORF("Zero size beginState");
    }

    if (!endState_.GetSize())
    {
        LOGERRORF("Zero size endState");
    }
}

bool SerializableEditOp::Undo()
{    
    if (!serializable_->Serializable::Load(beginState_))
    {
        LOGERRORF("Error loading beginState");
    }
    beginState_.Seek(0);

    VariantMap eventData;
    eventData[SceneEditSerializableUndoRedo::P_SERIALIZABLE] = serializable_;
    eventData[SceneEditSerializableUndoRedo::P_UNDO] = true;
    eventData[SceneEditSerializableUndoRedo::P_STATE] = beginState_;
    serializable_->SendEvent(E_SCENEEDITSERIALIZABLEUNDOREDO, eventData);

    return true;
}

bool SerializableEditOp::Redo()
{

    if (!serializable_->Serializable::Load(endState_))
    {
        LOGERRORF("Error loading endState");
    }

    endState_.Seek(0);

    VariantMap eventData;
    eventData[SceneEditSerializableUndoRedo::P_SERIALIZABLE] = serializable_;
    eventData[SceneEditSerializableUndoRedo::P_UNDO] = false;
    eventData[SceneEditSerializableUndoRedo::P_STATE] = endState_;
    serializable_->SendEvent(E_SCENEEDITSERIALIZABLEUNDOREDO, eventData);

    return true;
}

}
