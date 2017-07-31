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

#include <ThirdParty/TurboBadger/tb_layout.h>

#include "UIWidget.h"

namespace Atomic
{
 
/// A grid of buttons that keep their shape and size, with minimal programming.
class ATOMIC_API UIButtonGrid : public UIWidget
{
    ATOMIC_OBJECT(UIButtonGrid, UIWidget)

public:

    UIButtonGrid(Context* context, int numRows, int numCols, int margin=0, bool createWidget = true );
    virtual ~UIButtonGrid();

    virtual String GetGridId( int row, int col ); /// returns id text at r,c, ie, "A1" for 0,0
    virtual void SetGridText ( int row, int col, String str );  /// set button text at r,c location
    virtual String GetGridText( int row, int col ); /// returns button text at r,c
    virtual UIWidget* GetGridWidget( int row, int col ); /// returns widget at r,c
    virtual int AddGridText ( String str ); /// add strings starting at 0,0 and filling out cols, then next row... , returns count
    virtual void ResetAddCount() { m_count = 0; } /// resets the counter for AddGridText()
    virtual UIWidget* AtGridWidget( int count ); /// returns widget at count
    virtual String AtGridText( int count ); /// returns text at count
    virtual void DisableEmptyButtons(); /// convience function to disable buttons with no text, and enable those with text.
    virtual int GetNumRows() const;  /// returns number of rows that were programmed
    virtual int GetNumCols() const;  /// returns number of cols that were programmed
    virtual int GetRowHeight() const;  /// returns current row height
    virtual int GetColWidth() const;   /// returns current col width
    virtual int GetMargin() const;  /// returns current margin value

protected:

   virtual bool OnEvent(const tb::TBWidgetEvent &ev);
   virtual void OnResized(int old_w, int old_h);

private:

    void GenerateGrid();
    void ResizeGrid();
    virtual void AddGridRow( int rownum );

    int m_rows;
    int m_cols;
    int m_margin;
    int m_rowHeight;
    int m_colWidth;
    int m_count;

};

}
