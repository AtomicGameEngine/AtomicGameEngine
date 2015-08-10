
#pragma once

#include "UIWidget.h"
#include "UISelectList.h"

namespace Atomic
{

class ListViewItemSource;
class ListViewItem;

class UIListView : public UIWidget
{
    OBJECT(UIListView);

public:
    /// Construct.
    UIListView(Context* context, bool createWidget = true);

    /// Destruct.
    virtual ~UIListView();

    unsigned AddRootItem(const String& text, const String& icon, const String& id);

    unsigned AddChildItem(unsigned parentItemID, const String& text, const String& icon, const String& id);


    void SetItemText(const String& id, const String& text);
    void SetItemTextSkin(const String& id, const String& skin);
    void SetItemIcon(const String& id, const String& icon);    
    void DeleteItemByID(const String& id);

    void SetExpanded(unsigned itemID, bool value);

    void DeleteAllItems();
    void SelectItemByID(const String& id);

    String GetHoverItemID() { return rootList_.Null() ? "" : rootList_->GetHoverItemID(); }
    String GetSelectedItemID() { return rootList_.Null() ? "" : rootList_->GetSelectedItemID(); }

    UISelectList* GetRootList() { return rootList_; }

private:

    SharedPtr<UISelectList> rootList_;
    ListViewItemSource* source_;

    HashMap<unsigned,ListViewItem*> itemLookup_;

    unsigned itemLookupId_;

};

}
