// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"
#include "UIMenubar.h"

#include <Atomic/Scene/Scene.h>

using namespace Atomic;

namespace tb
{
    class TBLayout;
    class TBMenuWindow;
    class TBInlineSelect;
}

namespace AtomicEditor
{

class ListView;
class ListViewItem;
class InspectorDataBinding;

class InspectorFrame : public AEWidget
{

    OBJECT(InspectorFrame);

public:
    /// Construct.
    InspectorFrame(Context* context);
    /// Destruct.
    virtual ~InspectorFrame();

    bool OnEvent(const TBWidgetEvent &ev);

private:

    void InitializeSources();

    void InspectNode(Node* node);

    void HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData);

    TBLayout* inspectorContainer_;

    Vector<InspectorDataBinding*> dataBindings_;

    SharedPtr<Node> node_;

    MenubarItemSource componentCreateSource;
    MenubarItemSource audioCreateSource;
    MenubarItemSource geometryCreateSource;
    MenubarItemSource logicCreateSource;
    MenubarItemSource navigationCreateSource;
    MenubarItemSource networkCreateSource;
    MenubarItemSource physicsCreateSource;
    MenubarItemSource sceneCreateSource;
    MenubarItemSource subsystemCreateSource;

};

}
