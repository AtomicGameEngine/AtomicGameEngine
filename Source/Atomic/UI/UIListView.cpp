//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include <TurboBadger/tb_menu_window.h>
#include <TurboBadger/tb_select.h>

#include <Atomic/IO/Log.h>
#include <Atomic/Core/Timer.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIListView.h"

using namespace tb;

namespace Atomic
{

class ListViewItemSource;
class ListViewItemWidget;


class ListViewItemWidget : public TBLayout
{
public:
    ListViewItemWidget(ListViewItem *item, ListViewItemSource *source, TBSelectItemViewer *sourceviewer, int index);
    virtual bool OnEvent(const TBWidgetEvent &ev);

    void UpdateText(const String& text)
    {
        if (textField_)
            textField_->SetText(text.CString());
    }

    void UpdateIcon(const String& icon)
    {
        if (icon_)
            icon_->SetSkinBg(TBIDC(icon.CString()));
    }

    void UpdateTextSkin(const String& skin)
    {
        if (textField_)
            textField_->SetSkinBg(TBIDC(skin.CString()));
    }

    void SetExpanded(bool expanded)
    {
        if (expandBox_)
            expandBox_->SetValue(expanded ? 1 : 0);

    }


private:
    TBCheckBox* expandBox_;
    TBTextField* textField_;
    TBSkinImage* icon_;
    ListViewItemSource *source_;
    TBSelectItemViewer *sourceviewer_;
    int index_;
    ListViewItem* item_;
};


class ListViewItem : public TBGenericStringItem
{
    bool expanded_;
    bool selected_;

public:
    ListViewItem(const char *str, const TBID &id, const char* icon,  ListViewItemSource* source)
        : TBGenericStringItem(str, id), source_(source), parent_(0),
          depth_(0), widget_(0), expanded_(false), icon_(icon), selected_(false)
    {

    }

    ListViewItem* AddChild(const char* text, const char* icon, const TBID &id);

    bool GetSelected() { return selected_; }
    void SetSelected(bool value)
    {
        selected_ = value;
    }

    bool GetExpanded() { return expanded_; }

    void GetChildren(PODVector<ListViewItem*>& children, bool recursive = false)
    {
        children += children_;

        if (recursive)
        {
            for (unsigned i = 0; i < children_.Size(); i++)
            {
                children_[i]->GetChildren(children, recursive);
            }
        }
    }

    void SetExpanded(bool expanded)
    {
        if (widget_)
            widget_->SetExpanded(expanded);

        expanded_ = expanded;

        if (!expanded_)
        {
            //for (unsigned i = 0; i < children_.Size(); i ++)
            //    children_[i]->SetExpanded(expanded);
        }
        else
        {
            if (parent_)
                parent_->SetExpanded(expanded_);
        }
    }

    void UpdateText(const String& text);
    void UpdateTextSkin(const String& skin);
    void UpdateIcon(const String& icon);

    ListViewItemSource* source_;
    ListViewItem* parent_;
    int depth_;
    PODVector<ListViewItem*> children_;
    ListViewItemWidget* widget_;
    String icon_;
    String textSkin_;
};


class ListViewItemSource : public TBSelectItemSourceList<ListViewItem>
{
public:
    UIListView* uiListView_;
    ListViewItemSource(UIListView* list) : uiListView_(list) {}
    virtual ~ListViewItemSource() {}
    virtual bool Filter(int index, const char *filter);
    virtual TBWidget *CreateItemWidget(int index, TBSelectItemViewer *viewer);
};

// implementation

void ListViewItem::UpdateText(const String& text)
{
    str = text.CString();
    if (widget_)
        widget_->UpdateText(text);
}

void ListViewItem::UpdateIcon(const String& icon)
{
    icon_ = icon;
    if (widget_)
        widget_->UpdateIcon(icon);
}

void ListViewItem::UpdateTextSkin(const String& skin)
{
    textSkin_ = skin;
    if (widget_)
        widget_->UpdateTextSkin(skin);
}


ListViewItem* ListViewItem::AddChild(const char *text, const char* icon, const TBID &id)
{
    ListViewItem* child = new ListViewItem(text, id, icon, source_);
    child->parent_ = this;
    child->depth_ = depth_ + 1;

    // filter, alphabetically into source

    ListViewItem* insert = this;

    int parentIndex = -1;
    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        if (source_->GetItem(i) == this)
        {
            parentIndex = i;
            break;
        }
    }

    for (int i = parentIndex + 1; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);

        if (item->depth_ <= depth_)
            break;

        if (item->depth_ != depth_ + 1)
        {
            insert = item;
            continue;
        }

        if (strcmp(item->str.CStr(), text) >= 0)
            break;

        insert = item;

    }

    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        if (source_->GetItem(i) == insert)
        {
            source_->AddItem(child, i + 1);
            break;
        }
    }

    children_.Push(child);
    return child;
}


ListViewItemWidget::ListViewItemWidget(ListViewItem *item, ListViewItemSource *source,
                                       TBSelectItemViewer *sourceviewer, int index)
    : source_(source)
    , sourceviewer_(sourceviewer)
    , index_(index)
    , item_(item)
    , expandBox_(0)
    , textField_(0)
    , icon_(0)
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

    if (item_->children_.Size())
    {
        expandBox_ = new TBCheckBox();
        expandBox_->SetSkinBg(TBIDC("TBCheckBox.uilistview"));
        expandBox_->SetValue(item_->GetExpanded());
        expandBox_->SetID(item->id);
        GetContentRoot()->AddChild(expandBox_);
    }
    else
    {
        LayoutParams lp;
        lp.SetWidth(12);
        lp.SetHeight(4);
        TBWidget* spacer = new TBWidget();
        spacer->SetLayoutParams(lp);
        GetContentRoot()->AddChild(spacer);
    }

    if (item->icon_.Length())
    {
        icon_ = new TBSkinImage(TBIDC(item->icon_.CString()));
        icon_->SetIgnoreInput(true);
        GetContentRoot()->AddChild(icon_);
    }

    TBFontDescription fd;
    fd.SetID(TBIDC("Vera"));
    fd.SetSize(11);

    TBTextField* tfield = textField_ = new TBTextField();
    tfield->SetIgnoreInput(true);

    tfield->SetSkinBg(item->textSkin_.Length() ? TBIDC(item->textSkin_.CString()) : TBIDC("Folder"));
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

    if (ev.type == EVENT_TYPE_POINTER_DOWN || ev.type == EVENT_TYPE_RIGHT_POINTER_UP)
    {
        SetFocus(WIDGET_FOCUS_REASON_POINTER);

        TBWidget* parent = GetParent();

        while (parent)
        {
            if (parent->IsOfType<TBSelectList>())
            {
                TBWidgetEvent nev = ev;
                nev.ref_id = item_->id;
                parent->InvokeEvent(nev);
                break;
            }

            parent = parent->GetParent();
        }

        return true;
    }

    // get clicks this way, not sure we want to
    if (ev.type == EVENT_TYPE_CLICK &&  ev.target == expandBox_ && ev.target->GetID() == item_->id)
    {
        item_->SetExpanded(!item_->GetExpanded());

        source_->uiListView_->UpdateItemVisibility();

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

    return true;

}

TBWidget *ListViewItemSource::CreateItemWidget(int index, TBSelectItemViewer *viewer)
{
    ListViewItem* item = GetItem(index);

    if (TBLayout *layout = new ListViewItemWidget(item, this, viewer, index))
        return layout;

    return nullptr;
}

/*
static int select_list_sort_cb(TBSelectItemSource *_source, const int *a, const int *b)
{
    ListViewItemSource* source = (ListViewItemSource*) _source;

    ListViewItem* itemA = source->GetItem(*a);
    ListViewItem* itemB = source->GetItem(*b);

    int value;

    if (itemA->depth_ == itemB->depth_)
    {
        value = strcmp(source->GetItemString(*a), source->GetItemString(*b));
    }
    else
    {
        value = itemA->depth_ > itemB->depth_ ? 1 : -1;
    }

    return source->GetSort() == TB_SORT_DESCENDING ? -value : value;
}
*/

UIListView::UIListView(Context* context, bool createWidget) :
    UIWidget(context, createWidget),
    source_(0), itemLookupId_(0), multiSelect_(false), moveDelta_(0.0f), pivot_(nullptr), pivotIndex_(0), startNewSelection_(true)
{
    rootList_ = new UISelectList(context);
    rootList_->SetUIListView(true);

    // dummy filter so filter is called
    rootList_->SetFilter(" ");

    widget_->SetGravity(WIDGET_GRAVITY_ALL);
    rootList_->SetGravity(UI_GRAVITY_ALL);

    source_ = new ListViewItemSource(this);

    rootList_->GetTBSelectList()->SetSource(source_);

    AddChild(rootList_);
}

UIListView::~UIListView()
{

}

unsigned UIListView::AddRootItem(const String& text, const String& icon, const String& id)
{
    ListViewItem* item = new ListViewItem(text.CString(), TBID(id.CString()), icon.CString(), source_);
    source_->AddItem(item);

    itemLookup_[itemLookupId_++] = item;
    return itemLookupId_ - 1;
}

void UIListView::SetItemText(const String& id, const String& text)
{
    TBID tbid(id.CString());

    for (int i = 0; i <  source_->GetNumItems(); i++)
    {
        if (source_->GetItemID(i) == tbid)
        {
            ListViewItem* item = source_->GetItem(i);
            item->UpdateText(text);
            return;
        }
    }

}

void UIListView::SetItemTextSkin(const String& id, const String& skin)
{
    TBID tbid(id.CString());

    for (int i = 0; i <  source_->GetNumItems(); i++)
    {
        if (source_->GetItemID(i) == tbid)
        {
            ListViewItem* item = source_->GetItem(i);
            item->UpdateTextSkin(skin);
            return;
        }
    }

}

void UIListView::SetItemIcon(const String& id, const String& icon)
{
    TBID tbid(id.CString());

    for (int i = 0; i <  source_->GetNumItems(); i++)
    {
        if (source_->GetItemID(i) == tbid)
        {
            ListViewItem* item = source_->GetItem(i);
            item->UpdateIcon(icon);
            return;
        }
    }
}

void UIListView::DeleteItemByID(const String& id)
{
    TBID tbid(id.CString());

    for (int i = 0; i <  source_->GetNumItems(); i++)
    {
        if (source_->GetItemID(i) == tbid)
        {
            ListViewItem* item = source_->GetItem(i);

            if (item->parent_)
                item->parent_->children_.Remove(item);

            PODVector<ListViewItem*> children;

            item->GetChildren(children, true);

            for (unsigned j = 0; j < children.Size(); j++)
            {
                for (int k = 0; k <  source_->GetNumItems(); k++)
                {
                    if (children[j] == source_->GetItem(k))
                    {
                        source_->DeleteItem(k);
                        break;

                    }
                }
            }

            source_->DeleteItem(i);

            rootList_->InvalidateList();

            return;
        }
    }
}

unsigned UIListView::AddChildItem(unsigned parentItemID, const String& text, const String& icon, const String& id)
{
    if (!itemLookup_.Contains(parentItemID))
        return -1;

    ListViewItem* item = itemLookup_[parentItemID];

    ListViewItem* child = item->AddChild(text.CString(), icon.CString(), TBID(id.CString()));

    itemLookup_[itemLookupId_++] = child;

    return itemLookupId_ - 1;

}

void UIListView::SetExpanded(unsigned itemID, bool value)
{
    if (!itemLookup_.Contains(itemID))
        return;

    itemLookup_[itemID]->SetExpanded(value);

}

bool UIListView::GetExpanded(unsigned itemID)
{
    if (!itemLookup_.Contains(itemID))
        return false;

    return itemLookup_[itemID]->GetExpanded();

}

bool UIListView::GetExpandable(unsigned itemID)
{
    if (!itemLookup_.Contains(itemID))
        return false;

    return itemLookup_[itemID]->children_.Size() > 0;
}


void UIListView::DeleteAllItems()
{
    itemLookup_.Clear();
    source_->DeleteAllItems();
}


void UIListView::SelectItemByID(const String& id, bool selected)
{
    TBID tid = TBIDC(id.CString());

    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);

        if (tid == item->id)
        {
            if (selected)
            {
                if (item->GetSelected())
                    return;

                item->SetSelected(selected);
                if (item->parent_)
                    item->parent_->SetExpanded(true);
                SetValueFirstSelected();
                UpdateItemVisibility();
                ScrollToSelectedItem();
            }
            else
            {
                if (!item->GetSelected())
                    return;

                item->SetSelected(false);
                UpdateItemVisibility();

            }

            return;
        }

    }
}

void UIListView::UpdateItemVisibility()
{
    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);

        if (!item->widget_)
            continue;

        item->widget_->SetVisibilility(WIDGET_VISIBILITY_VISIBLE);
        item->widget_->SetState(WIDGET_STATE_SELECTED, item->GetSelected());

        ListViewItem* parent = item->parent_;
        while (parent)
        {
            if (!parent->GetExpanded())
                break;

            parent = parent->parent_;
        }

        if (parent)
            item->widget_->SetVisibilility(WIDGET_VISIBILITY_GONE);
    }

    tb::TBScrollContainer* scroll = (tb::TBScrollContainer*) rootList_->GetInternalWidget()->GetFirstChild();
    scroll->OnProcess();
}

void UIListView::ScrollToSelectedItem()
{
    if (rootList_.Null())
        return;

    rootList_->ScrollToSelectedItem();
}

void UIListView::SelectAllItems(bool select)
{
    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);
        item->SetSelected(select);
    }

}

void UIListView::SetValueFirstSelected()
{
    int index = -1;

    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);
        if (item->GetSelected())
        {
            index = i;
            break;
        }
    }

    rootList_->SetValue(index);

}

void UIListView::SelectSingleItem(ListViewItem* item, bool expand)
{

    if (!item)
        return;

    bool dirty = !item->GetSelected();

    if (!dirty)
    {
        for (unsigned i = 0; i < source_->GetNumItems(); i++)
        {
            ListViewItem* sitem = source_->GetItem(i);

            if (sitem != item && sitem->GetSelected())
            {
                dirty = true;
                break;
            }
        }
    }

    if (!dirty)
        return;

    for (unsigned i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* sitem = source_->GetItem(i);

        if (sitem->GetSelected())
        {
            sitem->SetSelected(false);
            SendItemSelectedChanged(sitem);
        }

    }

    if (expand)
        item->SetExpanded(true);

    item->SetSelected(true);
    UpdateItemVisibility();

    SetValueFirstSelected();
    ScrollToSelectedItem();

    SendItemSelectedChanged(item);

}

void UIListView::Move(tb::SPECIAL_KEY key)
{
    const float delta = 0.015f;
    if (moveDelta_)
    {
        Time* time = GetSubsystem<Time>();
        moveDelta_ -= time->GetTimeStep();
        if (moveDelta_ < 0.0f)
            moveDelta_ = 0.0f;
    }

    if (moveDelta_ > 0.0f)
        return;

    // selected index
    int index = -1;

    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);
        if (item->GetSelected())
        {
            index = i;
            break;
        }
    }

    // nothing selected
    if (index == -1)
        return;

    if (key == TB_KEY_LEFT)
    {
        ListViewItem* item = source_->GetItem(index);
        if (item->children_.Size() > 0 && item->GetExpanded())
        {
            item->SetExpanded(false);
            UpdateItemVisibility();
            moveDelta_ = delta;
            return;
        }
        else
        {
            if (!item->parent_)
                return;

            SelectSingleItem(item->parent_, false);
            moveDelta_ = delta;
            return;
        }
    }

    if (key == TB_KEY_RIGHT)
    {
        ListViewItem* item = source_->GetItem(index);
        if (item->children_.Size() > 0 && !item->GetExpanded())
        {
            item->SetExpanded(true);
            UpdateItemVisibility();
            moveDelta_ = delta;
            return;
        }
        else
        {
            if (!item->children_.Size())
                return;

            SelectSingleItem(source_->GetItem(index + 1), false);
            moveDelta_ = delta;
            return;

        }
    }


    if (key == TB_KEY_UP)
    {
        // can't go any further up list
        if (index == 0)
            return;

        for (int i = (int) (index - 1 ); i >= 0; i--)
        {
            ListViewItem* item = source_->GetItem(i);
            if (item->widget_ && item->widget_->GetVisibility() == WIDGET_VISIBILITY_VISIBLE)
            {
                SelectSingleItem(item, false);
                moveDelta_ = delta;
                return;
            }

        }

    }

    if (key == TB_KEY_DOWN)
    {
        // can't go any further down list
        if (index == source_->GetNumItems() - 1)
            return;

        for (int i = index + 1; i < source_->GetNumItems(); i++)
        {
            ListViewItem* item = source_->GetItem(i);
            if (item->widget_ && item->widget_->GetVisibility() == WIDGET_VISIBILITY_VISIBLE)
            {
                SelectSingleItem(item, false);
                moveDelta_ = delta;
                return;
            }

        }

    }

}

void UIListView::SendItemSelectedChanged(ListViewItem* item)
{
    UI* ui = GetSubsystem<UI>();

    VariantMap eventData;
    String refid;

    ui->GetTBIDString(item->id, refid);

    eventData[UIListViewSelectionChanged::P_REFID] = refid;
    eventData[UIListViewSelectionChanged::P_SELECTED] = item->GetSelected();
    this->SendEvent(E_UILISTVIEWSELECTIONCHANGED, eventData);

}

void UIListView::SelectItem(ListViewItem* item, bool select)
{
    item->SetSelected(select);
    SendItemSelectedChanged(item);
}

bool UIListView::OnEvent(const tb::TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_KEY_UP )
    {
        moveDelta_ = 0.0f;
    }

    if (ev.type == EVENT_TYPE_KEY_DOWN )
    {
        if (ev.special_key == TB_KEY_DOWN || ev.special_key == TB_KEY_UP || ev.special_key == TB_KEY_LEFT || ev.special_key == TB_KEY_RIGHT)
        {
            Move(ev.special_key);
            return true;
        }
    }

    if (ev.type == EVENT_TYPE_CUSTOM && ev.ref_id == TBIDC("select_list_validation_end"))
    {
        UpdateItemVisibility();
        return true;
    }

    if (ev.type == EVENT_TYPE_CUSTOM && ev.ref_id == TBIDC("select_list_selection_changed"))
    {
        for (int i = 0; i < source_->GetNumItems(); i++)
        {
            ListViewItem* item = source_->GetItem(i);

            if (item->id == ev.target->GetID())
            {
                bool multi = false;
                if (multiSelect_ && (ev.modifierkeys & TB_CTRL || ev.modifierkeys & TB_SUPER))
                    multi = true;

                bool shiftMulti = false;
                if (multiSelect_ && (ev.modifierkeys & TB_SHIFT))
                    shiftMulti = true;

                if (shiftMulti)
                {
                    if (startNewSelection_)
                        SelectAllItems(false);

                    if (!pivot_)
                    {
                        pivotIndex_ = 0;
                        pivot_ = source_->GetItem(pivotIndex_);
                    }

                    SetValueFirstSelected();

                    if (i > pivotIndex_)
                    {
                        for (int j = pivotIndex_; j < i; j++)
                        {
                            ListViewItem* itemSelect = source_->GetItem(j);

                            if (!itemSelect->parent_ || itemSelect->parent_->GetExpanded())
                                SelectItem(itemSelect, true);
                        }
                    }
                    else if (i < pivotIndex_)
                    {
                        for (int j = pivotIndex_; j > i; j--)
                        {
                            ListViewItem* itemSelect = source_->GetItem(j);

                            if (itemSelect->parent_->GetExpanded())
                                SelectItem(itemSelect, true);
                        }
                    }

                    SelectItem(item, true);
                    UpdateItemVisibility();
                }
                else if (multi)
                {
                    if (item->GetSelected())
                    {
                        SelectItem(item, false);
                    }
                    else
                    {
                        SelectItem(item, true);
                    }

                    SetValueFirstSelected();
                    UpdateItemVisibility();

                    pivot_ = item;
                    pivotIndex_ = i;
                    startNewSelection_ = false;
                }
                else
                {
                    SelectSingleItem(item, false);

                    pivot_ = item;
                    pivotIndex_ = i;
                    startNewSelection_ = true;
                }

                return true;
            }

        }
    }

    if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        return false;
    }

    return UIWidget::OnEvent(ev);
}

}
