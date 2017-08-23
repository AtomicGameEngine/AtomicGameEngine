//
// Copyright (c) 2014-2017, THUNDERBEAST GAMES LLC All rights reserved
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

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_select_item.h>
#include <TurboBadger/tb_atomic_widgets.h>

#include "../Core/Object.h"
#include "../Container/List.h"
#include "../IO/Log.h"

#include "UISelectItem.h"

namespace Atomic
{

class UIMultiItemSource;

class ATOMIC_API UIMultiItem : public  UISelectItem
{
    ATOMIC_OBJECT(UIMultiItem, UISelectItem )

public:

    UIMultiItem(Context* context, const String& colid,const String& widgettype, const String& str, int colwidth, int colheight );
    virtual ~UIMultiItem();

    void SetID(const String& id);
    tb::TBID GetID() { return id_; }

    void AddColumn ( const String& widgettype, const String& str, int colwidth );
    const String& GetColumnStr( int col );
    const String& GetColumnWidget( int col );
    int GetColumnWidth( int col );
    int GetNumColumns();

    virtual tb::MultiItem * GetTBItem();

protected:

    tb::TBID id_;    // TBID
    Vector <String> colStr_;
    Vector <String> colWidget_; 
    Vector <int>  colWidth_;
    int colHeight_; 
};

class ATOMIC_API UIMultiItemSource : public UISelectItemSource
{
    ATOMIC_OBJECT(UIMultiItemSource, UISelectItemSource )

public:

    UIMultiItemSource(Context* context);
    virtual ~UIMultiItemSource();

    void AddItem(UIMultiItem* item) { items_.Push(SharedPtr<UIMultiItem>(item)); }
    void RemoveItemWithId(const String& id);
    void RemoveItemWithStr(const String& str);
    int GetItemCount() { return items_.Size(); }

    void Clear() { items_.Clear(); }

    /// Returns item string for the index. Returns empty string for invalid indexes.
    const String& GetItemStr(int index);

    // caller's responsibility to clean up
    virtual tb::MultiItemSource* GetTBItemSource();

protected:

    List<SharedPtr<UIMultiItem>> items_;

};

}
