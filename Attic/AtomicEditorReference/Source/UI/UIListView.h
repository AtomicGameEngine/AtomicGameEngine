// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "AEWidget.h"

#include <TurboBadger/tb_menu_window.h>
#include <TurboBadger/tb_select.h>

using namespace Atomic;

namespace AtomicEditor
{

class ListViewItemSource;
class ListViewItemWidget;

class ListViewItem : public TBGenericStringItem
{
    bool expanded_;

public:
    ListViewItem(const char *str, const TBID &id, const char* icon,  ListViewItemSource* source)
        : TBGenericStringItem(str, id), source_(source), parent_(0),
          depth_(0), widget_(0), expanded_(false), icon_(icon)
    {
    }

    ListViewItem* AddChild(const char* text, const char* icon, const TBID &id);

    bool GetExpanded() { return expanded_; }

    void SetExpanded(bool expanded)
    {
        expanded_ = expanded;
        if (!expanded_)
        {
            for (unsigned i = 0; i < children_.Size(); i ++)
                children_[i]->SetExpanded(expanded);
        }
        else
        {
            ListViewItem* p = parent_;
            while (p)
            {
                p->expanded_ = true;
                p = p->parent_;
            }
        }
    }

    ListViewItemSource* source_;
    ListViewItem* parent_;
    int depth_;
    PODVector<ListViewItem*> children_;
    ListViewItemWidget* widget_;
    String icon_;
};

class ListViewItemWidget : public TBLayout
{
public:
    ListViewItemWidget(ListViewItem *item, ListViewItemSource *source, TBSelectItemViewer *sourceviewer, int index);
    virtual bool OnEvent(const TBWidgetEvent &ev);
private:
    ListViewItemSource *source_;
    TBSelectItemViewer *sourceviewer_;
    int index_;
    ListViewItem* item_;
};


class ListViewItemSource : public TBSelectItemSourceList<ListViewItem>
{
public:
    TBSelectList* list_;
    ListViewItemSource(TBSelectList* list) : list_(list) {}
    virtual ~ListViewItemSource() {}
    virtual bool Filter(int index, const char *filter);
    virtual TBWidget *CreateItemWidget(int index, TBSelectItemViewer *viewer);
};


class ListView : public AEWidget
{
    OBJECT(ListView);

    TBSelectList* rootList_;
    ListViewItemSource* source_;

public:
    /// Construct.
    ListView(Context* context, const TBID& id);
    /// Destruct.
    virtual ~ListView();

    ListViewItem* AddItem(const char* text, const char* icon, const TBID &id)
    {
        ListViewItem* item = new ListViewItem(text, id, icon, source_);
        source_->AddItem(item);
        return item;
    }

    ListViewItem* GetItem(int index)
    {
        return source_->GetItem(index);
    }

    int GetNumItems()
    {
        return source_->GetNumItems();
    }

    void DeleteAllItems()
    {
        source_->DeleteAllItems();
    }

    TBSelectList* GetRootList() { return rootList_; }

    void SelectItemByID(const TBID &id);

    bool OnEvent(const TBWidgetEvent &ev);

private:

};

}
