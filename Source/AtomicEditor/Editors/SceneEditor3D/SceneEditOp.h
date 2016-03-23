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
#include <Atomic/IO/VectorBuffer.h>

namespace Atomic
{
class Node;
class Component;
}

using namespace Atomic;

namespace AtomicEditor
{

enum SceneEditType
{
    SCENEEDIT_UNKNOWN = 0,
    SCENEEDIT_SELECTION
};

class SceneEditOp
{

public:

    SceneEditOp(Scene* scene, SceneEditType type) { type_ = type; scene_ = scene;}
    virtual ~SceneEditOp() { }

    virtual bool Undo() = 0;
    virtual bool Redo() = 0;

    /// Returns true if the states are identical
    bool CompareStates(const VectorBuffer& stateOne, const VectorBuffer& stateTwo) const
    {
        if (stateOne.GetSize() != stateTwo.GetSize())
            return false;

        if (memcmp(stateOne.GetData(), stateTwo.GetData(), stateOne.GetSize()))
            return false;

        return true;
    }

    SharedPtr<Scene> scene_;
    SceneEditType type_;

};

class SelectionEditOp : public SceneEditOp
{

public:

    SelectionEditOp(Scene* scene);
    ~SelectionEditOp();

    bool Undo();
    bool Redo();

    void RegisterEdit();

    void SetNodes(Vector<SharedPtr<Node>>& nodes);

    void AddNode(Node* node);

    void NodeAdded(Node* node, Node* parent);
    void NodeRemoved(Node* node, Node* parent);

    // Erases a node from the edit op, return true if no other nodes in the operation
    bool EraseNode(Node *node);

    bool Commit();

private:

    struct EditComponent
    {
        SharedPtr<Component> component_;
        SharedPtr<Node> nodeBegin_;
        SharedPtr<Node> nodeEnd_;
        VectorBuffer stateBegin_;
        VectorBuffer stateEnd_;
    };

    struct EditNode
    {
        SharedPtr<Node> node_;
        SharedPtr<Node> parentBegin_;
        SharedPtr<Node> parentEnd_;
        VectorBuffer stateBegin_;
        VectorBuffer stateEnd_;
        PODVector<EditComponent*> components_;
    };

    PODVector<EditNode*> editNodes_;

};

}
