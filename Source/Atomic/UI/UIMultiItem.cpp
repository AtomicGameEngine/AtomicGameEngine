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

#include "UIMultiItem.h"

using namespace tb;

namespace Atomic
{

UIMultiItem::UIMultiItem(Context* context, const String &colid, const String &widgettype, const String &str, int colwidth, int colheight )
 : UISelectItem(context, str, colid)
{
    SetID(colid);
    AddColumn ( widgettype, str, colwidth );
    colHeight_ = colheight; 
}

UIMultiItem::~UIMultiItem()
{
}

void UIMultiItem::SetID(const String& id)
{
    id_ = TBID(id.CString());
}

void UIMultiItem::AddColumn ( const String& widgettype, const String& str, int cwidth ) 
{
    colStr_.Push(str);
    colWidget_.Push(widgettype);
    colWidth_.Push(cwidth);
}

const String& UIMultiItem::GetColumnStr( int col )
{
    if ( col > -1 )
        return colStr_[col];
    return ( String::EMPTY );
}

const String& UIMultiItem::GetColumnWidget( int col ) 
{
    if ( col > -1 )
    return colWidget_[col];
    return ( String::EMPTY );
}

int UIMultiItem::GetColumnWidth( int col ) 
{
    if ( col > -1 )
        return colWidth_[col];
    return 0;
}

int UIMultiItem::GetNumColumns()
{
    return colStr_.Size();
}

tb::MultiItem* UIMultiItem::GetTBItem()
{
    tb::MultiItem* item = NULL;

    int col = 0;
    int numcols = GetNumColumns();
    String strx = GetColumnStr(col);
    String widx = GetColumnWidget(col);
    int wix = GetColumnWidth(col);
    item = new tb::MultiItem(id_, widx.CString(), strx.CString(), wix, colHeight_ );
    for ( col = 1; col<numcols; col++)
    {
        strx = GetColumnStr(col);
        widx = GetColumnWidget(col);
        wix = GetColumnWidth(col);
        item->AddColumn( widx.CString(), strx.CString(), wix );
    }
    return item;
}

// UIMultiItemSource 

UIMultiItemSource::UIMultiItemSource(Context* context) :  UISelectItemSource(context)
{

}

UIMultiItemSource::~UIMultiItemSource()
{

}

void UIMultiItemSource::RemoveItemWithId(const String& id)
{
    tb::TBID test = TBID(id.CString());
    for (List<SharedPtr<UIMultiItem> >::Iterator itr = items_.Begin(); itr != items_.End(); itr++)
    {
        if ((*itr)->GetID() == test) {
            items_.Erase(itr);
            break;
        }
    }
}

void UIMultiItemSource::RemoveItemWithStr(const String& str)
{
    for (List<SharedPtr<UIMultiItem> >::Iterator itr = items_.Begin(); itr != items_.End(); itr++)
    {
        if ((*itr)->GetColumnStr(0) == str) {
            items_.Erase(itr);
            break;
        }
    }
}

const String& UIMultiItemSource::GetItemStr(int index)
{
    int nn = 0;
    for (List<SharedPtr<UIMultiItem> >::Iterator itr = items_.Begin(); itr != items_.End(); itr++)
    {
        if ( nn == index) return (*itr)->GetColumnStr(0);
        nn++;
    }
    return ( String::EMPTY );
}

MultiItemSource *UIMultiItemSource::GetTBItemSource()
{
    // caller's responsibility to clean up
    MultiItemSource* src = new MultiItemSource();

    for (List<SharedPtr<UIMultiItem> >::Iterator itr = items_.Begin(); itr != items_.End(); itr++)
    {
        tb::MultiItem* tbitem = (*itr)->GetTBItem();
        src->AddItem(tbitem);
    }

    return src;
}

}
