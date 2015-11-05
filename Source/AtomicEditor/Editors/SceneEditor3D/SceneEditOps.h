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

}

using namespace Atomic;

namespace AtomicEditor
{

enum SceneHistoryOpType
{
    SCENEHISTORYOP_UNKNOWN = 0,
    SCENEHISTORYOP_NODEADDED,
    SCENEHISTORYOP_NODEREMOVED,
    SCENEHISTORYOP_NODECHANGED,

    SCENEHISTORYOP_COMPONENTCHANGED
};

class SceneHistoryOp
{

public:

    SceneHistoryOp() { type_ = SCENEHISTORYOP_UNKNOWN; }
    virtual ~SceneHistoryOp() { }

    virtual bool Undo() = 0;
    virtual bool Redo() = 0;

    SceneHistoryOpType type_;

};

class NodeEditOp : public SceneHistoryOp
{
protected:

    NodeEditOp(Node* node);

public:

    SharedPtr<Node> node_;
    SharedPtr<Node> parent_;

};

class NodeAddedOp : public NodeEditOp
{

public:

    NodeAddedOp(Node* node);

    bool Undo();
    bool Redo();
};

class NodeChangedOp : public NodeEditOp
{

public:

    NodeChangedOp(Node* node, const VectorBuffer& prevState);

    bool Undo();
    bool Redo();

    VectorBuffer prevState_;
    VectorBuffer newState_;

};

class ComponentEditOp : public SceneHistoryOp
{
protected:

    ComponentEditOp(Component* component);

public:

    SharedPtr<Component> component_;

};

class ComponentChangedOp : public ComponentEditOp
{

public:

    ComponentChangedOp(Component* component, const VectorBuffer& prevState);

    bool Undo();
    bool Redo();

    VectorBuffer prevState_;
    VectorBuffer newState_;

};


}
