
#pragma once

#include "UIWidget.h"
#include "UISelectList.h"

namespace Atomic
{

class ListViewItemSource;

class UIListView : public UIWidget
{
    OBJECT(UIListView);

public:
    /// Construct.
    UIListView(Context* context, bool createWidget = true);

    /// Destruct.
    virtual ~UIListView();

    void AddItem(const String& text, const String& icon, const String& id);
    void DeleteAllItems();
    void SelectItemByID(const String& id);

    UISelectList* GetRootList() { return rootList_; }

private:

    SharedPtr<UISelectList> rootList_;

    ListViewItemSource* source_;

};

}
