// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <TurboBadger/tb_select.h>

#include <Atomic/Core/Context.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceEvents.h>

#include <Atomic/UI/UI.h>

#include "AEEditor.h"
#include "AEEvents.h"

#include "UIListView.h"
#include "UIHierarchyFrame.h"

#include "UI/Modal/UIModalOps.h"

using namespace tb;

namespace AtomicEditor
{

HierarchyFrame::HierarchyFrame(Context* context) :
    AEWidget(context)
{
    UI* tbui = GetSubsystem<UI>();
    tbui->LoadResourceFile(delegate_, "AtomicEditor/editor/ui/hierarchyframe.tb.txt");

    delegate_->SetID(TBIDC("projectframe_delegate"));

    delegate_->SetGravity(WIDGET_GRAVITY_TOP_BOTTOM);

    // folders
    TBLayout* hierarchycontainer = delegate_->GetWidgetByIDAndType<TBLayout>(TBIDC("hierarchycontainer"));
    assert(hierarchycontainer);

    hierarchyList_ = new ListView(context_, "hierarchyList_");
    hierarchyList_->GetRootList()->SetGravity(WIDGET_GRAVITY_ALL);

    TBWidgetDelegate* hierarchyListWD = hierarchyList_->GetWidgetDelegate();

    hierarchyListWD->SetID(TBIDC("hierarchyList_delegate"));

    hierarchyListWD->SetGravity(WIDGET_GRAVITY_ALL);

    hierarchycontainer->AddChild(hierarchyListWD);

    SubscribeToEvent(E_EDITORACTIVESCENECHANGE, HANDLER(HierarchyFrame, HandleEditorActiveSceneChange));

}

HierarchyFrame::~HierarchyFrame()
{

}

void HierarchyFrame::RecursiveAddNode(ListViewItem* parent, Node* node)
{

    String name = node->GetName();
    if (!name.Length())
        name = "(Anonymous)";

    ListViewItem* child = parent->AddChild(name.CString(), "Folder.icon", TBID(uint32(node->GetID())));

    for (unsigned i = 0; i < node->GetNumChildren(); i++)
        RecursiveAddNode(child, node->GetChild(i));
}


void HierarchyFrame::RefreshHierarchyList()
{
    hierarchyList_->DeleteAllItems();

    if (scene_.Null())
        return;

    ListViewItem* parent = hierarchyList_->AddItem("Scene", "Folder.icon", TBID(uint32(scene_->GetID())));

    for (unsigned i = 0; i < scene_->GetNumChildren(); i++)
        RecursiveAddNode(parent, scene_->GetChild(i));

    hierarchyList_->GetRootList()->SetValue(0);
    hierarchyList_->GetItem(0)->SetExpanded(true);

}

bool HierarchyFrame::OnEvent(const TBWidgetEvent &ev)
{
    Editor* editor = GetSubsystem<Editor>();
    if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target->GetID() == TBIDC("menu create"))
        {

            editor->PostModalInfo("Early Access - Not Implemented", "Node creation not implemented in this Early Access build");

        }

        if (scene_.NotNull())
        {
            if (ev.target->GetID() == TBIDC("hierarchyList_"))
            {
                unsigned id = ev.ref_id;
                Node* node = scene_->GetNode(id);
                if (node)
                {
                    VariantMap neventData;
                    neventData[EditorActiveNodeChange::P_NODE] = node;
                    SendEvent(E_EDITORACTIVENODECHANGE, neventData);
                }
            }
        }
    }

    return false;
}

void HierarchyFrame::HandleEditorActiveSceneChange(StringHash eventType, VariantMap& eventData)
{
    Scene* scene = (Scene*) (eventData[EditorActiveSceneChange::P_SCENE].GetPtr());

    scene_ = scene;
    if (scene_)
    {
        RefreshHierarchyList();
    }

    VariantMap neventData;
    neventData[EditorActiveNodeChange::P_NODE] = scene_;
    SendEvent(E_EDITORACTIVENODECHANGE, neventData);

}

}
