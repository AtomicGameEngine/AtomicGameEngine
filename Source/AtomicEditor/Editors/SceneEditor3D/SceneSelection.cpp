
#include <Atomic/IO/Log.h>
#include <Atomic/Core/CoreEvents.h>

#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Drawable.h>
#include <Atomic/Graphics/DebugRenderer.h>
#include <Atomic/Atomic3D/Terrain.h>

#include <Atomic/Scene/SceneEvents.h>
#include <Atomic/Scene/Node.h>
#include <Atomic/Scene/Scene.h>

#include "SceneEditor3D.h"
#include "SceneEditor3DEvents.h"
#include "SceneSelection.h"

namespace AtomicEditor
{

SceneSelection::SceneSelection(Context* context, SceneEditor3D *sceneEditor) : Object(context)
{
    sceneEditor3D_ = sceneEditor;
    scene_ = sceneEditor3D_->GetScene();

    SubscribeToEvent(E_POSTRENDERUPDATE, HANDLER(SceneSelection, HandlePostRenderUpdate));
    SubscribeToEvent(scene_, E_NODEREMOVED, HANDLER(SceneSelection, HandleNodeRemoved));
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

void SceneSelection::RemoveNode(Node* node)
{    
    SharedPtr<Node> _node(node);
    if(!nodes_.Contains(_node))
        return;

    nodes_.Remove(_node);

    VariantMap eventData;
    eventData[SceneNodeSelected::P_SCENE] = scene_;
    eventData[SceneNodeSelected::P_NODE] = node;
    eventData[SceneNodeSelected::P_SELECTED] = false;    
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
    /*
    if (clipboardNode_.NotNull() && selectedNode_.NotNull())
    {
        SharedPtr<Node> pasteNode(clipboardNode_->Clone());

        VariantMap eventData;
        eventData[EditorActiveNodeChange::P_NODE] = pasteNode;
        SendEvent(E_EDITORACTIVENODECHANGE, eventData);

        VariantMap editData;
        editData[SceneEditNodeAddedRemoved::P_SCENE] = scene_;
        editData[SceneEditNodeAddedRemoved::P_NODE] = pasteNode;
        editData[SceneEditNodeAddedRemoved::P_ADDED] = true;

        scene_->SendEvent(E_SCENEEDITNODEADDEDREMOVED, editData);
    }
    */

}


void SceneSelection::Copy()
{
    /*

    if (selectedNode_.NotNull())
    {
        clipboardNode_ = selectedNode_;
    }

    */
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
    RemoveNode(node);
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


}
