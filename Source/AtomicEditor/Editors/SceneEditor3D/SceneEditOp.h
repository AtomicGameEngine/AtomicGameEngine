//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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

