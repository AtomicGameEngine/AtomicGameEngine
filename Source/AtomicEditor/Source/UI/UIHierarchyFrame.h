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
}

namespace AtomicEditor
{

class ListView;
class ListViewItem;

class HierarchyFrame : public AEWidget
{

    OBJECT(HierarchyFrame);

public:
    /// Construct.
    HierarchyFrame(Context* context);
    /// Destruct.
    virtual ~HierarchyFrame();

    void RefreshHierarchyList();

    bool OnEvent(const TBWidgetEvent &ev);

private:
    void HandleEditorActiveSceneChange(StringHash eventType, VariantMap& eventData);

    void RecursiveAddNode(ListViewItem* parent, Node* node);

    SharedPtr<ListView> hierarchyList_;

    SharedPtr<Scene> scene_;
};

}
