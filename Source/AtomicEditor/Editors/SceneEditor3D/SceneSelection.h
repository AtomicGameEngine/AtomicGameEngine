//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace Atomic
{
class Node;
class Scene;
}

namespace AtomicEditor
{

class SceneEditor3D;

class SceneSelection: public Object
{
    OBJECT(SceneSelection);

public:

    SceneSelection(Context* context, SceneEditor3D* sceneEditor);
    virtual ~SceneSelection();

    Vector<SharedPtr<Node>>& GetNodes() { return nodes_; }

    void Cut();
    void Copy();
    void Paste();
    void Delete();

    /// Add a node to the selection, if clear specified removes current nodes first
    void AddNode(Node* node, bool clear = false);
    void RemoveNode(Node* node, bool quiet = false);
    void GetBounds(BoundingBox& bbox);

    bool Contains(Node* node);

    Node* GetSelectedNode(unsigned index) const;
    unsigned GetSelectedNodeCount() const { return nodes_.Size(); }

    void Clear();

private:

    void DrawNodeDebug(Node* node, DebugRenderer* debug, bool drawNode = true);

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData);
    void HandleNodeRemoved(StringHash eventType, VariantMap& eventData);

    void HandleSceneEditPrefabSave(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditPrefabRevert(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditPrefabBreak(StringHash eventType, VariantMap& eventData);

    WeakPtr<SceneEditor3D> sceneEditor3D_;
    WeakPtr<Scene> scene_;

    Vector<SharedPtr<Node>> clipBoardNodes_;
    Vector<SharedPtr<Node>> nodes_;

};

}
