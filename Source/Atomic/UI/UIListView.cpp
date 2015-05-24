
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

// implementation
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

//-----------------------------------------------------------------------------------

UIListView::UIListView(Context* context, bool createWidget) :
    UIWidget(context, createWidget),
    source_(0)
{
    rootList_ = new UISelectList(context);

    // dummy filter so filter is called
    rootList_->SetFilter(" ");

    widget_->SetGravity(WIDGET_GRAVITY_ALL);
    rootList_->SetGravity(WIDGET_GRAVITY_ALL);

    source_ = new ListViewItemSource(rootList_->GetTBSelectList());

    rootList_->GetTBSelectList()->SetSource(source_);

    AddChild(rootList_);
}

UIListView::~UIListView()
{

}

void UIListView::AddItem(const String& text, const String& icon, const String& id)
{
    ListViewItem* item = new ListViewItem(text.CString(), TBID(id.CString()), icon.CString(), source_);
    source_->AddItem(item);
}

void UIListView::DeleteAllItems()
{

}


void UIListView::SelectItemByID(const String& id)
{

}


}
