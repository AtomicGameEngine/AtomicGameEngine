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

#include <Atomic/IO/Log.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Component.h>
#include <Atomic/Scene/PrefabComponent.h>
#include <Atomic/Scene/Scene.h>

#include "SceneEditOp.h"
#include "SceneEditor3DEvents.h"

namespace AtomicEditor
{

SelectionEditOp::SelectionEditOp(Scene *scene) : SceneEditOp(scene, SCENEEDIT_SELECTION)
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

bool SelectionEditOp::EraseNode(Node *node)
{
    PODVector<EditNode*>::Iterator itr = editNodes_.Begin();

    while (itr != editNodes_.End())
    {
        if ((*itr)->node_ == node)
        {
            for (unsigned j = 0; j < (*itr)->components_.Size(); j++)
                delete (*itr)->components_[j];

            delete *itr;
            editNodes_.Erase(itr);
            break;
        }

        itr++;
    }

    return editNodes_.Size() == 0;
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

            if (ecomponent->nodeBegin_ != ecomponent->nodeEnd_)
                return true;

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

    scene_->SendEvent(E_SCENEEDITSTATECHANGESBEGIN);

    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        EditNode* enode = editNodes_[i];

        Node* node = enode->node_;

        bool changed = !CompareStates(enode->stateBegin_, enode->stateEnd_);
        if (changed && !node->Serializable::Load(enode->stateBegin_))
        {
            ATOMIC_LOGERRORF("Unable to Undo node serializable");
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
            if(enode->parentBegin_.NotNull())
            {
                // moving back to original parent

                if (node->GetParent())
                {
                    VariantMap nodeRemovedEventData;
                    nodeRemovedEventData[SceneEditNodeRemoved::P_NODE] = node;
                    nodeRemovedEventData[SceneEditNodeRemoved::P_PARENT] =  node->GetParent();
                    nodeRemovedEventData[SceneEditNodeRemoved::P_SCENE] = scene_;
                    scene_->SendEvent(E_SCENEEDITNODEREMOVED, nodeRemovedEventData);
                }

                node->Remove();
                enode->parentBegin_->AddChild(node);

                VariantMap nodeAddedEventData;
                nodeAddedEventData[SceneEditNodeAdded::P_NODE] = node;
                nodeAddedEventData[SceneEditNodeAdded::P_PARENT] = enode->parentBegin_;
                nodeAddedEventData[SceneEditNodeAdded::P_SCENE] = scene_;
                scene_->SendEvent(E_SCENEEDITNODEADDED, nodeAddedEventData);

            }
            else
            {
                VariantMap nodeRemovedEventData;
                nodeRemovedEventData[SceneEditNodeRemoved::P_NODE] = node;
                nodeRemovedEventData[SceneEditNodeRemoved::P_PARENT] =  enode->parentEnd_;
                nodeRemovedEventData[SceneEditNodeRemoved::P_SCENE] = scene_;
                scene_->SendEvent(E_SCENEEDITNODEREMOVED, nodeRemovedEventData);
                node->Remove();
            }

        }

        for (unsigned j = 0; j < enode->components_.Size(); j++)
        {
            EditComponent* ecomponent = enode->components_[j];
            Component* component = ecomponent->component_;

            changed = !CompareStates(ecomponent->stateBegin_, ecomponent->stateEnd_);
            if (changed && !component->Serializable::Load(ecomponent->stateBegin_))
            {
                ATOMIC_LOGERRORF("Unable to Undo component serializable");
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

                bool add = ecomponent->nodeBegin_.NotNull();

                VariantMap caData;
                caData[SceneEditComponentAddedRemoved::P_SCENE] = scene_;
                caData[SceneEditComponentAddedRemoved::P_COMPONENT] = component;

                if (add)
                {
                    ecomponent->nodeBegin_->AddComponent(component, 0, REPLICATED);
                    caData[SceneEditComponentAddedRemoved::P_NODE] = ecomponent->nodeBegin_;
                    caData[SceneEditComponentAddedRemoved::P_REMOVED] = false;
                }
                else
                {
                    caData[SceneEditComponentAddedRemoved::P_NODE] = ecomponent->nodeEnd_;
                    caData[SceneEditComponentAddedRemoved::P_REMOVED] = true;
                }

                scene_->SendEvent(E_SCENEEDITCOMPONENTADDEDREMOVED, caData);
            }

        }

    }

    scene_->SendEvent(E_SCENEEDITSTATECHANGESEND);

    return true;
}

bool SelectionEditOp::Redo()
{
    scene_->SendEvent(E_SCENEEDITSTATECHANGESBEGIN);

    for (unsigned i = 0; i < editNodes_.Size(); i++)
    {
        EditNode* enode = editNodes_[i];

        Node* node = enode->node_;

        bool changed = !CompareStates(enode->stateBegin_, enode->stateEnd_);
        if ( changed && !node->Serializable::Load(enode->stateEnd_))
        {
            ATOMIC_LOGERRORF("Unable to Redo node serializable");
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

            if(enode->parentEnd_.NotNull())
            {
                if (node->GetParent())
                {
                    VariantMap nodeRemovedEventData;
                    nodeRemovedEventData[SceneEditNodeRemoved::P_NODE] = node;
                    nodeRemovedEventData[SceneEditNodeRemoved::P_PARENT] =  node->GetParent();
                    nodeRemovedEventData[SceneEditNodeRemoved::P_SCENE] = scene_;
                    scene_->SendEvent(E_SCENEEDITNODEREMOVED, nodeRemovedEventData);
                }

                node->Remove();
                enode->parentEnd_->AddChild(node);

                VariantMap nodeAddedEventData;
                nodeAddedEventData[SceneEditNodeAdded::P_NODE] = node;
                nodeAddedEventData[SceneEditNodeAdded::P_PARENT] = enode->parentEnd_;
                nodeAddedEventData[SceneEditNodeAdded::P_SCENE] = scene_;
                scene_->SendEvent(E_SCENEEDITNODEADDED, nodeAddedEventData);

            }
            else
            {
                VariantMap nodeRemovedEventData;
                nodeRemovedEventData[SceneEditNodeRemoved::P_NODE] = node;
                nodeRemovedEventData[SceneEditNodeRemoved::P_PARENT] = enode->parentBegin_;
                nodeRemovedEventData[SceneEditNodeRemoved::P_SCENE] = scene_;
                scene_->SendEvent(E_SCENEEDITNODEREMOVED, nodeRemovedEventData);
                node->Remove();
            }

        }

        for (unsigned j = 0; j < enode->components_.Size(); j++)
        {
            EditComponent* ecomponent = enode->components_[j];
            Component* component = ecomponent->component_;

            changed = !CompareStates(ecomponent->stateBegin_, ecomponent->stateEnd_);
            if ( changed && !component->Serializable::Load(ecomponent->stateEnd_))
            {
                ATOMIC_LOGERRORF("Unable to Redo component serializable");
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

                bool add = ecomponent->nodeEnd_.NotNull();

                VariantMap caData;
                caData[SceneEditComponentAddedRemoved::P_SCENE] = scene_;
                caData[SceneEditComponentAddedRemoved::P_COMPONENT] = component;

                if (add)
                {
                    ecomponent->nodeEnd_->AddComponent(component, 0, REPLICATED);
                    caData[SceneEditComponentAddedRemoved::P_NODE] = ecomponent->nodeEnd_;
                    caData[SceneEditComponentAddedRemoved::P_REMOVED] = false;
                }
                else
                {
                    caData[SceneEditComponentAddedRemoved::P_NODE] = ecomponent->nodeBegin_;
                    caData[SceneEditComponentAddedRemoved::P_REMOVED] = true;
                }

                scene_->SendEvent(E_SCENEEDITCOMPONENTADDEDREMOVED, caData);

            }

        }

    }

    scene_->SendEvent(E_SCENEEDITSTATECHANGESEND);

    return true;
}


}
