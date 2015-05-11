// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <Atomic/UI/UI.h>
#include <Atomic/IO/Log.h>
#include "UIListView.h"

using namespace tb;


namespace AtomicEditor
{

ListViewItem* ListViewItem::AddChild(const char *text, const char* icon, const TBID &id)
{
    ListViewItem* child = new ListViewItem(text, id, icon, source_);
    child->parent_ = this;
    child->depth_ = depth_ + 1;
    source_->AddItem(child);
    children_.Push(child);
    return child;
}


ListViewItemWidget::ListViewItemWidget(ListViewItem *item, ListViewItemSource *source,
                                       TBSelectItemViewer *sourceviewer, int index)
    : source_(source)
    , sourceviewer_(sourceviewer)
    , index_(index)
    , item_(item)
{
    SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    SetLayoutDistributionPosition(LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP);
    SetPaintOverflowFadeout(false);

    item_->widget_ = this;

    for (int i = 0; i < item->depth_; i++)
    {
        LayoutParams lp;
        lp.SetWidth(6);
        lp.SetHeight(4);
        TBWidget* spacer = new TBWidget();
        spacer->SetLayoutParams(lp);
        GetContentRoot()->AddChild(spacer);
    }

    if (item->icon_.Length())
    {
        TBSkinImage* skinImage = new TBSkinImage(TBIDC(item->icon_.CString()));
        skinImage->SetIgnoreInput(true);
        GetContentRoot()->AddChild(skinImage);
    }

    TBFontDescription fd;
    fd.SetID(TBIDC("Vera"));
    fd.SetSize(11);

    TBTextField* tfield = new TBTextField();
    tfield->SetIgnoreInput(true);
    tfield->SetSkinBg(TBIDC("Folder"));
    tfield->SetText(item->str);
    tfield->SetFontDescription(fd);

    SetSkinBg(TBIDC("TBSelectItem"));
    GetContentRoot()->AddChild(tfield);

    SetID(item->id);
}

bool ListViewItemWidget::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_WHEEL)
    {
        return false;
    }

    // get clicks this way, not sure we want to
    if (ev.type == EVENT_TYPE_CLICK && ev.target->GetID() == item_->id)
    {
        item_->SetExpanded(!item_->GetExpanded());
        source_->list_->InvalidateList();

        // want to bubble
        return false;
    }

    return TBLayout::OnEvent(ev);
}


bool ListViewItemSource::Filter(int index, const char *filter)
{
    ListViewItem* item = GetItem(index);

    if (!item->parent_)
        return true;

    if (item->parent_->GetExpanded())
        return true;

    return false;

}

TBWidget *ListViewItemSource::CreateItemWidget(int index, TBSelectItemViewer *viewer)
{
    ListViewItem* item = GetItem(index);

    if (TBLayout *layout = new ListViewItemWidget(item, this, viewer, index))
        return layout;

    return nullptr;
}


ListView::ListView(Context* context, const TBID& id) :
    AEWidget(context),
    rootList_(0),
    source_(0)
{
    rootList_ = new TBSelectList();
    rootList_->SetID(id);


    // dummy filter so filter is called
    rootList_->SetFilter(" ");

    delegate_->SetGravity(WIDGET_GRAVITY_ALL);
    rootList_->SetGravity(WIDGET_GRAVITY_ALL);
    source_ = new ListViewItemSource(rootList_);

    rootList_->SetSource(source_);

    delegate_->AddChild(rootList_);
}

ListView::~ListView()
{

}

void ListView::SelectItemByID(const TBID &id)
{
    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);

        if (id == item->id)
        {
            item->SetExpanded(true);
            rootList_->SetValue(i);
            rootList_->InvalidateList();
            return;
        }

    }

}

bool ListView::OnEvent(const TBWidgetEvent &ev)
{
    return false;
}

}
