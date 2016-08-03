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
    ATOMIC_OBJECT(SceneSelection, Object);

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
    void HandleSceneEditPrefabCopy(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditPrefabPaste(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditComponentCopy(StringHash eventType, VariantMap& eventData);
    void HandleSceneEditComponentPaste(StringHash eventType, VariantMap& eventData);

    WeakPtr<SceneEditor3D> sceneEditor3D_;
    WeakPtr<Scene> scene_;

    Vector<SharedPtr<Node>> clipBoardNodes_;
    Vector<SharedPtr<Node>> nodes_;

    Component* copiedComponent_;

    Vector3 nodePosition_;
    Quaternion nodeRotation_;
    Vector3 nodeScale_;

    Vector<String> componentAttributeNames_;
    Vector<Variant> componentAttributeValues_;

    bool hasCopied_;

};

}
