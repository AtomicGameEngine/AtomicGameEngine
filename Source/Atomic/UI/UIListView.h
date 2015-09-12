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
