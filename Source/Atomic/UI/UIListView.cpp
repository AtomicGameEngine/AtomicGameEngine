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

#include "UIListView.h"

using namespace tb;

namespace Atomic
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


private:
    TBCheckBox* expandBox_;
    TBTextField* textField_;
    TBSkinImage* icon_;
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
    SetCapturing(false);

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
    source_(0), itemLookupId_(0)
{
    rootList_ = new UISelectList(context);

    // dummy filter so filter is called
    rootList_->SetFilter(" ");

    widget_->SetGravity(WIDGET_GRAVITY_ALL);
    rootList_->SetGravity(UI_GRAVITY_ALL);

    source_ = new ListViewItemSource(rootList_->GetTBSelectList());

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


void UIListView::SelectItemByID(const String& id)
{
    TBID tid = TBIDC(id.CString());

    for (int i = 0; i < source_->GetNumItems(); i++)
    {
        ListViewItem* item = source_->GetItem(i);

        if (tid == item->id)
        {
            //item->SetExpanded(true);
            rootList_->SetValue(i);
            rootList_->InvalidateList();
            return;
        }

    }
}

void UIListView::ScrollToSelectedItem()
{
    if (rootList_.Null())
        return;

    rootList_->ScrollToSelectedItem();
}


}
