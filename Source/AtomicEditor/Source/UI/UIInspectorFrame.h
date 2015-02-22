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

    struct DataBinding
    {
        TBWidget* widget;
        Component* component;
        unsigned attrIndex;
    };

    void Clear();

    void InspectNode(Node* node);

    void CreateTransformLayout();

    TBLayout* CreateComponentLayout(Component *component);
    void RefreshTransform();

    void HandleEditorActiveNodeChange(StringHash eventType, VariantMap& eventData);

    TBLayout* inspectorContainer_;

    TBLayout* transformLayout_;
    TBInlineSelect* posXSelect_;
    TBInlineSelect* posYSelect_;
    TBInlineSelect* posZSelect_;
    TBInlineSelect* rotXSelect_;
    TBInlineSelect* rotYSelect_;
    TBInlineSelect* rotZSelect_;
    TBInlineSelect* scaleXSelect_;
    TBInlineSelect* scaleYSelect_;
    TBInlineSelect* scaleZSelect_;

    Vector<DataBinding> dataBindings_;

    bool refreshing_;

    SharedPtr<Node> node_;

};

}
