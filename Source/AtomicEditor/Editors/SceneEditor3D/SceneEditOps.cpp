
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Component.h>

#include "SceneEditOps.h"

namespace AtomicEditor
{

NodeEditOp::NodeEditOp(Node* node)
{
    node_ = node;

    // parent at the time of the operation
    parent_ = node->GetParent();
}

NodeAddedOp::NodeAddedOp(Node* node) : NodeEditOp(node)
{
    type_ = SCENEHISTORYOP_NODEADDED;

}

bool NodeAddedOp::Undo()
{
    node_->Remove();
    return true;
}

bool NodeAddedOp::Redo()
{
    parent_->AddChild(node_);
    return true;
}

NodeChangedOp::NodeChangedOp(Node* node, const VectorBuffer& prevState) : NodeEditOp(node)
{
    type_ = SCENEHISTORYOP_NODECHANGED;

    prevState_ = prevState;
    node->Animatable::Save(newState_);
    newState_.Seek(0);
}

bool NodeChangedOp::Undo()
{
    node_->Animatable::Load(prevState_);
    prevState_.Seek(0);
    return true;
}

bool NodeChangedOp::Redo()
{
    node_->Animatable::Load(newState_);
    newState_.Seek(0);
    return true;
}

//------------ Component Ops

ComponentEditOp::ComponentEditOp(Component* component)
{
    component_ = component;

}

ComponentChangedOp::ComponentChangedOp(Component* component, const VectorBuffer& prevState) : ComponentEditOp(component)
{
    type_ = SCENEHISTORYOP_COMPONENTCHANGED;

    prevState_ = prevState;
    component_->Animatable::Save(newState_);
    newState_.Seek(0);
}

bool ComponentChangedOp::Undo()
{
    component_->Animatable::Load(prevState_);
    prevState_.Seek(0);
    return true;
}

bool ComponentChangedOp::Redo()
{
    component_->Animatable::Load(newState_);
    newState_.Seek(0);
    return true;
}



}
