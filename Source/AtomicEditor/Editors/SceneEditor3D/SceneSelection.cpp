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
#include <Atomic/Core/CoreEvents.h>

#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Drawable.h>
#include <Atomic/Graphics/DebugRenderer.h>
#include <Atomic/Graphics/Terrain.h>

#include <Atomic/Scene/SceneEvents.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Scene.h>
#include <Atomic/Scene/PrefabComponent.h>

#include "SceneEditor3D.h"
#include "SceneEditor3DEvents.h"
#include "SceneSelection.h"
#include "SceneEditHistory.h"

namespace AtomicEditor
{

SceneSelection::SceneSelection(Context* context, SceneEditor3D *sceneEditor) : Object(context), hasCopied_(false)
{
    sceneEditor3D_ = sceneEditor;
    scene_ = sceneEditor3D_->GetScene();

    SubscribeToEvent(E_POSTRENDERUPDATE, ATOMIC_HANDLER(SceneSelection, HandlePostRenderUpdate));
    SubscribeToEvent(scene_, E_NODEREMOVED, ATOMIC_HANDLER(SceneSelection, HandleNodeRemoved));

    SubscribeToEvent(scene_, E_SCENEEDITPREFABSAVE, ATOMIC_HANDLER(SceneSelection, HandleSceneEditPrefabSave));
    SubscribeToEvent(scene_, E_SCENEEDITPREFABREVERT, ATOMIC_HANDLER(SceneSelection, HandleSceneEditPrefabRevert));
    SubscribeToEvent(scene_, E_SCENEEDITPREFABBREAK, ATOMIC_HANDLER(SceneSelection, HandleSceneEditPrefabBreak));
    SubscribeToEvent(scene_, E_SCENEEDITPREFABCOPY, ATOMIC_HANDLER(SceneSelection, HandleSceneEditPrefabCopy));
    SubscribeToEvent(scene_, E_SCENEEDITPREFABPASTE, ATOMIC_HANDLER(SceneSelection, HandleSceneEditPrefabPaste));
    SubscribeToEvent(scene_, E_SCENEEDITCOMPONENTCOPY, ATOMIC_HANDLER(SceneSelection, HandleSceneEditComponentCopy));
    SubscribeToEvent(scene_, E_SCENEEDITCOMPONENTPASTE, ATOMIC_HANDLER(SceneSelection, HandleSceneEditComponentPaste));
}

SceneSelection::~SceneSelection()
{

}

Node* SceneSelection::GetSelectedNode(unsigned index) const
{
    if (index > nodes_.Size())
        return 0;

    return nodes_[index];
}

bool SceneSelection::Contains(Node* node)
{
    SharedPtr<Node> _node(node);
    return nodes_.Contains(_node);
}

void SceneSelection::AddNode(Node* node, bool clear)
{
    if (clear)
        Clear();

    SharedPtr<Node> _node(node);
    if (!nodes_.Contains(_node))
    {
        nodes_.Push(_node);

        VariantMap eventData;
        eventData[SceneNodeSelected::P_SCENE] = scene_;
        eventData[SceneNodeSelected::P_NODE] = node;
        eventData[SceneNodeSelected::P_SELECTED] = true;
        scene_->SendEvent(E_SCENENODESELECTED, eventData);
    }
}

void SceneSelection::RemoveNode(Node* node, bool quiet)
{
    SharedPtr<Node> _node(node);
    if(!nodes_.Contains(_node))
        return;

    nodes_.Remove(_node);

    VariantMap eventData;
    eventData[SceneNodeSelected::P_SCENE] = scene_;
    eventData[SceneNodeSelected::P_NODE] = node;
    eventData[SceneNodeSelected::P_SELECTED] = false;
    eventData[SceneNodeSelected::P_QUIET] = quiet;
    scene_->SendEvent(E_SCENENODESELECTED, eventData);

}

void SceneSelection::Clear()
{
    Vector<SharedPtr<Node>> nodes = nodes_;

    for (unsigned i = 0; i < nodes.Size(); i++)
    {
        RemoveNode(nodes[i]);
    }

}

void SceneSelection::Paste()
{

    if (!clipBoardNodes_.Size())
        return;

    Vector<SharedPtr<Node>> newClipBoardNodes;

    Node* parent = scene_;

    if (nodes_.Size() >= 1)
        parent = nodes_[0]->GetParent();

    if (!parent)
        parent = scene_;

    Clear();

    VariantMap eventData;
    eventData[SceneEditAddRemoveNodes::P_END] = false;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

    for (unsigned i = 0; i < clipBoardNodes_.Size(); i++)
    {
        // Nodes must have a parent to clone, so first parent
        Node* clipNode = clipBoardNodes_[i];

        Matrix3x4 transform = clipNode->GetWorldTransform();

        parent->AddChild(clipNode);
        clipNode->SetWorldTransform(transform.Translation(), transform.Rotation(), transform.Scale());

        // clone
        newClipBoardNodes.Push(SharedPtr<Node>(clipNode->Clone()));
        // remove from parent
        newClipBoardNodes.Back()->Remove();
        newClipBoardNodes.Back()->SetWorldTransform(transform.Translation(), transform.Rotation(), transform.Scale());

        // generate scene edit event
        VariantMap nodeAddedEventData;
        nodeAddedEventData[SceneEditNodeAdded::P_NODE] = clipNode;
        nodeAddedEventData[SceneEditNodeAdded::P_PARENT] = parent;
        nodeAddedEventData[SceneEditNodeAdded::P_SCENE] = scene_;
        scene_->SendEvent(E_SCENEEDITNODEADDED, nodeAddedEventData);
    }

    eventData[SceneEditAddRemoveNodes::P_END] = true;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

    for (unsigned i = 0; i < clipBoardNodes_.Size(); i++)
    {
        AddNode(clipBoardNodes_[i], false);
    }

    clipBoardNodes_ = newClipBoardNodes;

}

void SceneSelection::Cut()
{
    Copy();
    Delete();
}

void SceneSelection::Copy()
{
    clipBoardNodes_.Clear();

    for (unsigned i = 0; i < nodes_.Size(); i++)
    {
        Node* node = nodes_[i];

        if (!node->GetParent())
        {
            clipBoardNodes_.Clear();
            ATOMIC_LOGERROR("SceneSelection::Copy - unable to copy node to clipboard (no parent)");
            return;
        }

        for (unsigned j = 0; j < nodes_.Size(); j++)
        {
            if ( i == j )
                continue;

            PODVector<Node*> children;
            nodes_[j]->GetChildren(children, true);
            if (children.Contains(node))
            {
                node = 0;
                break;
            }

        }

        if (node)
        {
            Matrix3x4 transform = node->GetWorldTransform();
            SharedPtr<Node> clipNode(node->Clone());
            clipNode->Remove();
            clipNode->SetWorldTransform(transform.Translation(), transform.Rotation(), transform.Scale());
            clipBoardNodes_.Push(clipNode);
        }

    }
}

void SceneSelection::Delete()
{

    Vector<SharedPtr<Node>> nodes = nodes_;

    Clear();

    VariantMap eventData;
    eventData[SceneEditAddRemoveNodes::P_END] = false;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

    for (unsigned i = 0; i < nodes.Size(); i++)
    {
        // generate scene edit event
        VariantMap nodeRemovedEventData;
        nodeRemovedEventData[SceneEditNodeAdded::P_NODE] = nodes[i];
        nodeRemovedEventData[SceneEditNodeAdded::P_PARENT] = nodes[i]->GetParent();
        nodeRemovedEventData[SceneEditNodeAdded::P_SCENE] = scene_;
        scene_->SendEvent(E_SCENEEDITNODEREMOVED, nodeRemovedEventData);

        nodes[i]->Remove();

    }

    eventData[SceneEditAddRemoveNodes::P_END] = true;
    scene_->SendEvent(E_SCENEEDITADDREMOVENODES, eventData);

}

void SceneSelection::GetBounds(BoundingBox& bbox)
{

    bbox.Clear();

    if (!nodes_.Size())
        return;

    // Get all the drawables, which define the bounding box of the selection
    PODVector<Drawable*> drawables;

    for (unsigned i = 0; i < nodes_.Size(); i++)
    {
        Node* node = nodes_[i];
        node->GetDerivedComponents<Drawable>(drawables, true, false);
    }

    if (!drawables.Size())
        return;

    // Calculate the combined bounding box of all drawables
    for (unsigned i = 0; i < drawables.Size(); i++  )
    {
        Drawable* drawable = drawables[i];
        bbox.Merge(drawable->GetWorldBoundingBox());
    }

}

void SceneSelection::DrawNodeDebug(Node* node, DebugRenderer* debug, bool drawNode)
{
    if (drawNode)
        debug->AddNode(node, 1.0, false);

    // Exception for the scene to avoid bringing the editor to its knees: drawing either the whole hierarchy or the subsystem-
    // components can have a large performance hit. Also do not draw terrain child nodes due to their large amount
    // (TerrainPatch component itself draws nothing as debug geometry)
    if (node != scene_ && !node->GetComponent<Terrain>())
    {
        const Vector<SharedPtr<Component> >& components = node->GetComponents();

        for (unsigned j = 0; j < components.Size(); ++j)
            components[j]->DrawDebugGeometry(debug, false);

        // To avoid cluttering the view, do not draw the node axes for child nodes
        for (unsigned k = 0; k < node->GetNumChildren(); ++k)
            DrawNodeDebug(node->GetChild(k), debug, false);
    }
}

void SceneSelection::HandleNodeRemoved(StringHash eventType, VariantMap& eventData)
{
    Node* node = (Node*) (eventData[NodeRemoved::P_NODE].GetPtr());

    RemoveNode(node, true);
}


void SceneSelection::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{

    if (!nodes_.Size())
        return;

    // Visualize the currently selected nodes
    DebugRenderer* debugRenderer = sceneEditor3D_->GetSceneView3D()->GetDebugRenderer();

    for (unsigned i = 0; i < nodes_.Size(); i++)
    {
        DrawNodeDebug(nodes_[i], debugRenderer);
    }

}

void SceneSelection::HandleSceneEditPrefabSave(StringHash eventType, VariantMap& eventData)
{

    Node* node = static_cast<Node*> ( eventData[SceneEditPrefabSave::P_NODE].GetPtr());

    PrefabComponent* prefab = node->GetComponent<PrefabComponent>();
    if (!prefab)
    {
        ATOMIC_LOGERRORF("Prefab Save: Unable to get prefab component for node: %s", node->GetName().CString());
        return;
    }

    prefab->SavePrefab();

    AddNode(node, true);
}

void SceneSelection::HandleSceneEditPrefabRevert(StringHash eventType, VariantMap& eventData)
{
    Node* node = static_cast<Node*> ( eventData[SceneEditPrefabRevert::P_NODE].GetPtr());

    PrefabComponent* prefab = node->GetComponent<PrefabComponent>();
    if (!prefab)
    {
        ATOMIC_LOGERRORF("Prefab Revert: Unable to get prefab component for node: %s", node->GetName().CString());
        return;
    }

    prefab->UndoPrefab();

    AddNode(node, true);
}

void SceneSelection::HandleSceneEditPrefabBreak(StringHash eventType, VariantMap& eventData)
{
    Node* node = static_cast<Node*> ( eventData[SceneEditPrefabBreak::P_NODE].GetPtr());

    PrefabComponent* prefab = node->GetComponent<PrefabComponent>();
    if (!prefab)
    {
        ATOMIC_LOGERRORF("Prefab Break: Unable to get prefab component for node: %s", node->GetName().CString());
        return;
    }

    Clear();

    prefab->BreakPrefab();

    PODVector<Node*> nodes;
    node->GetChildren(nodes, true);
    nodes.Insert(0, node);

    SceneEditHistory* editHistory = sceneEditor3D_->GetEditHistory();

    for (unsigned i = 0; i < nodes.Size(); i++)
    {
        editHistory->RemoveNode(nodes[i]);
    }

    AddNode(node, true);

    scene_->SendEvent(E_SCENEEDITSCENEMODIFIED);

}

void SceneSelection::HandleSceneEditPrefabCopy(StringHash eventType, VariantMap & eventData)
{
    Node* node = static_cast<Node*> (eventData[SceneEditPrefabCopy::P_NODE].GetPtr());

    PrefabComponent* prefab = node->GetComponent<PrefabComponent>();
    if (!prefab)
    {
        ATOMIC_LOGERRORF("Prefab Copy: Unable to get prefab component for node: %s", node->GetName().CString());
        return;
    }

    nodePosition_ = node->GetAttribute("Position").GetVector3();
    nodeRotation_ = node->GetAttribute("Rotation").GetQuaternion();
    nodeScale_ = node->GetAttribute("Scale").GetVector3();
}

void SceneSelection::HandleSceneEditPrefabPaste(StringHash eventType, VariantMap & eventData)
{
    Node* node = static_cast<Node*> (eventData[SceneEditPrefabPaste::P_NODE].GetPtr());

    PrefabComponent* prefab = node->GetComponent<PrefabComponent>();
    if (!prefab)
    {
        ATOMIC_LOGERRORF("Prefab Paste: Unable to get prefab component for node: %s", node->GetName().CString());
        return;
    }

    node->SetPosition(nodePosition_);
    node->SetRotation(nodeRotation_);
    node->SetScale(nodeScale_);
}

void SceneSelection::HandleSceneEditComponentCopy(StringHash eventType, VariantMap & eventData)
{
    Component* component = static_cast<Component*> (eventData[SceneEditComponentCopy::P_COMPONENT].GetPtr());
    copiedComponent_ = component;

    if (!component)
    {
        ATOMIC_LOGERRORF("Component Copy: Unable to copy component from node: %s", component->GetAttribute("type").ToString().CString());
        return;
    }

    componentAttributeNames_.Clear();
    componentAttributeValues_.Clear();

    for (unsigned i = 0; i < component->GetAttributes()->Size(); i++)
    {
        String attributeName = component->GetAttributes()->At(i).name_;
        Variant attributeValue = component->GetAttribute(attributeName);
        componentAttributeNames_.Push(attributeName);
        componentAttributeValues_.Push(attributeValue);
    }

    hasCopied_ = true;

}

void SceneSelection::HandleSceneEditComponentPaste(StringHash eventType, VariantMap & eventData)
{
    Component* component = static_cast<Component*> (eventData[SceneEditComponentCopy::P_COMPONENT].GetPtr());

    if (!component)
    {
        ATOMIC_LOGERRORF("Component Paste: Unable to paste component to node: %s", component->GetAttribute("type").ToString().CString());
        return;
    }

    if (!hasCopied_)
    {
        ATOMIC_LOGERROR("Component Paste: Unable to paste, no information has been copied.");
        return;
    }

    if (copiedComponent_->GetType() != component->GetType())
    {
        ATOMIC_LOGERROR("Component Paste: Unable to paste, component type differ.");
        return;
    }

    for (unsigned i = 0; i < componentAttributeNames_.Size(); i++)
    {
        String attrName = componentAttributeNames_[i];
        Variant attrValue = componentAttributeValues_[i];
        component->SetAttribute(attrName, attrValue);
    }
}


}
