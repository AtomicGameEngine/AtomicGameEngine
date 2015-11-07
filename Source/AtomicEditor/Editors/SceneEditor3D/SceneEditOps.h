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

class Scene;
class Node;
class Component;
class Serializable;

}

using namespace Atomic;

namespace AtomicEditor
{

enum SceneEditType
{
    SCENEEDIT_UNKNOWN = 0,
    SCENEEDIT_NODE_ADDED_REMOVED,
    SCENEEDIT_COMPONENT_ADDED_REMOVED,
    SCENEEDIT_SERIALIZABLE_EDIT,
};

class SceneEditOp
{

public:

    SceneEditOp() { type_ = SCENEEDIT_UNKNOWN; }
    virtual ~SceneEditOp() { }

    virtual bool Undo() = 0;
    virtual bool Redo() = 0;

    SceneEditType type_;

};

class NodeEditOp : public SceneEditOp
{
protected:

    NodeEditOp(Node* node);

public:

    SharedPtr<Node> node_;
    SharedPtr<Node> parent_;

};

class NodeAddedRemovedOp : public NodeEditOp
{

public:

    NodeAddedRemovedOp(Node* node, bool added);

    bool added_;

    bool Undo();
    bool Redo();
};

class ComponentEditOp : public SceneEditOp
{
protected:

    ComponentEditOp(Component* component);

public:

    SharedPtr<Component> component_;

};

class ComponentAddedRemovedOp : public ComponentEditOp
{

public:

    ComponentAddedRemovedOp(Node* node, bool added);

    bool added_;

    bool Undo();
    bool Redo();
};

class SerializableEditOp : public SceneEditOp
{

public:

    SerializableEditOp(Serializable* serializable, const VectorBuffer& beginState, const VectorBuffer& endState);

    SharedPtr<Serializable> serializable_;
    VectorBuffer beginState_;
    VectorBuffer endState_;

    bool Undo();
    bool Redo();
};


}
