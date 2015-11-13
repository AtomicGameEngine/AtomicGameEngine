
#include <Atomic/IO/Log.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Component.h>
#include <Atomic/Scene/Scene.h>

#include "SceneEditOp.h"
#include "SceneEditor3DEvents.h"

namespace AtomicEditor
{

SelectionEditOp::SelectionEditOp() : SceneEditOp(SCENEEDIT_SELECTION)
{
}

SelectionEditOp::~SelectionEditOp()
{

    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        EditNode* enode = editNodes_[i];
        for (unsigned j = 0; j < enode->components_.Size(); j++)
            delete enode->components_[j];
        delete enode;
    }

}

void SelectionEditOp::AddNode(Node* node)
{

    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        if (editNodes_[i]->node_ == node)
            return;
    }

    EditNode* enode = new EditNode();
    enode->node_ = node;
    enode->parentBegin_ = enode->parentEnd_ = node->GetParent();
    node->Serializable::Save(enode->stateBegin_);
    enode->stateBegin_.Seek(0);
    enode->stateEnd_ = enode->stateBegin_;

    const Vector<SharedPtr<Component>>& components = node->GetComponents();

    for (unsigned j = 0; j < components.Size(); j++)
    {
        Component* component = components[j];

        EditComponent* ecomponent = new EditComponent();
        ecomponent->component_ = component;
        ecomponent->nodeBegin_ = ecomponent->nodeEnd_ = node;
        component->Serializable::Save(ecomponent->stateBegin_);
        ecomponent->stateBegin_.Seek(0);
        ecomponent->stateEnd_ = ecomponent->stateBegin_;

        enode->components_.Push(ecomponent);
    }

    editNodes_.Push(enode);

}

void SelectionEditOp::NodeAdded(Node* node, Node* parent)
{
    AddNode(node);

    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        if (editNodes_[i]->node_ == node)
        {
            editNodes_[i]->parentBegin_ = 0;
            editNodes_[i]->parentEnd_ = parent;
            return;
        }
    }
}

void SelectionEditOp::NodeRemoved(Node* node, Node* parent)
{
    AddNode(node);

    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        if (editNodes_[i]->node_ == node)
        {
            editNodes_[i]->parentBegin_ = parent;
            editNodes_[i]->parentEnd_ = 0;
            return;
        }
    }
}

void SelectionEditOp::SetNodes(Vector<SharedPtr<Node> > &nodes)
{
    // Generate initial snapshot
    for (unsigned i = 0; i < nodes.Size(); i++)
    {
        AddNode(nodes[i]);

    }
}

bool SelectionEditOp::Commit()
{
    // See if any nodes, components have been edited
    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        EditNode* enode = editNodes_[i];

        if (enode->parentBegin_ != enode->parentEnd_)
            return true;

        if (!CompareStates(enode->stateBegin_, enode->stateEnd_))
            return true;

        for (unsigned j = 0; j < enode->components_.Size(); j++)
        {
            EditComponent* ecomponent = enode->components_[j];

            if (!CompareStates(ecomponent->stateBegin_, ecomponent->stateEnd_))
                return true;
        }

    }

    return false;

}

void SelectionEditOp::RegisterEdit()
{
    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        EditNode* enode = editNodes_[i];
        enode->stateEnd_.Clear();
        enode->node_->Serializable::Save(enode->stateEnd_);
        enode->stateEnd_.Seek(0);
        enode->parentEnd_ = enode->node_->GetParent();

        for (unsigned j = 0; j < enode->components_.Size(); j++)
        {
            EditComponent* ecomponent = enode->components_[j];
            ecomponent->stateEnd_.Clear();
            ecomponent->component_->Serializable::Save(ecomponent->stateEnd_);
            ecomponent->stateEnd_.Seek(0);
            ecomponent->nodeEnd_ = ecomponent->component_->GetNode();
        }

    }

}

bool SelectionEditOp::Undo()
{

    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        EditNode* enode = editNodes_[i];

        Node* node = enode->node_;

        bool changed = !CompareStates(enode->stateBegin_, enode->stateEnd_);
        if (changed && !node->Serializable::Load(enode->stateBegin_))
        {
            LOGERRORF("Unable to Undo node serializable");
            return false;
        }

        if (changed)
        {
            VariantMap eventData;
            eventData[SceneEditStateChange::P_SERIALIZABLE] = node;
            node->SendEvent(E_SCENEEDITSTATECHANGE, eventData);
        }

        enode->stateBegin_.Seek(0);

        if (node->GetParent() != enode->parentBegin_)
        {
            node->Remove();
            if(enode->parentBegin_.NotNull())
                enode->parentBegin_->AddChild(node);
        }

        for (unsigned j = 0; j < enode->components_.Size(); j++)
        {
            EditComponent* ecomponent = enode->components_[j];
            Component* component = ecomponent->component_;

            changed = !CompareStates(ecomponent->stateBegin_, ecomponent->stateEnd_);
            if (changed && !component->Serializable::Load(ecomponent->stateBegin_))
            {
                LOGERRORF("Unable to Undo component serializable");
                return false;
            }

            if (changed)
            {
                VariantMap eventData;
                eventData[SceneEditStateChange::P_SERIALIZABLE] = component;
                component->SendEvent(E_SCENEEDITSTATECHANGE, eventData);
            }


            ecomponent->stateBegin_.Seek(0);

            if (component->GetNode() != ecomponent->nodeBegin_)
            {
                component->Remove();
                if (ecomponent->nodeBegin_.NotNull())
                    ecomponent->nodeBegin_->AddComponent(component, 0, REPLICATED);
            }

        }

    }

    return true;
}

bool SelectionEditOp::Redo()
{
    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        EditNode* enode = editNodes_[i];

        Node* node = enode->node_;

        bool changed = !CompareStates(enode->stateBegin_, enode->stateEnd_);
        if ( changed && !node->Serializable::Load(enode->stateEnd_))
        {
            LOGERRORF("Unable to Redo node serializable");
            return false;
        }

        enode->stateEnd_.Seek(0);

        if (changed)
        {
            VariantMap eventData;
            eventData[SceneEditStateChange::P_SERIALIZABLE] = node;
            node->SendEvent(E_SCENEEDITSTATECHANGE, eventData);
        }

        if (node->GetParent() != enode->parentEnd_)
        {
            node->Remove();
            if(enode->parentEnd_.NotNull())
                enode->parentEnd_->AddChild(node);
        }

        for (unsigned j = 0; j < enode->components_.Size(); j++)
        {
            EditComponent* ecomponent = enode->components_[j];
            Component* component = ecomponent->component_;

            changed = !CompareStates(ecomponent->stateBegin_, ecomponent->stateEnd_);
            if ( changed && !component->Serializable::Load(ecomponent->stateEnd_))
            {
                LOGERRORF("Unable to Redo component serializable");
                return false;
            }

            ecomponent->stateEnd_.Seek(0);

            if (changed)
            {
                VariantMap eventData;
                eventData[SceneEditStateChange::P_SERIALIZABLE] = component;
                component->SendEvent(E_SCENEEDITSTATECHANGE, eventData);
            }

            if (component->GetNode() != ecomponent->nodeEnd_)
            {
                component->Remove();
                if (ecomponent->nodeEnd_.NotNull())
                {
                    ecomponent->nodeEnd_->AddComponent(component, 0, REPLICATED);
                }

            }

        }

    }

    return true;
}


}

