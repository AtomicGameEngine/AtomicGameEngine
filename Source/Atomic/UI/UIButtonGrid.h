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

    UIButtonGrid(Context* context, int numRows, int numColumns, int margin=0, bool createWidget = true );
    virtual ~UIButtonGrid();
    
    /// returns id text for a row,column location, ie, A1 for location 0,0
    String GetGridId( int row, int column );
    /// set the button text at the row,column location
    void SetGridText ( int row, int column, String str );
    /// returns the button text at the row,column location
    String GetGridText( int row, int column );
    /// returns widget at the row,column location
    UIWidget* GetGridWidget( int row, int column );
    /// adds button text strings starting at 0,0 and filling out by cols, then rows, returns count
    int AddGridText ( String str );
    /// resets the counter for AddGridText()
    void ResetAddCount() { addCount_ = 0; }
    /// returns the widget at count
    UIWidget* AtGridWidget( int count );
    /// returns the button text at count
    String AtGridText( int count );
    /// convience function to disable buttons with no text, and enable those with text.
    void DisableEmptyButtons();
    /// returns number of rows that were programmed
    int GetNumRows() const;
    /// returns number of columns that were programmed
    int GetNumColumns() const;
    /// returns current row height value
    int GetRowHeight() const;
    /// returns current column width value
    int GetColumnWidth() const;
    /// returns current margin value
    int GetMargin() const;

protected:

   virtual bool OnEvent(const tb::TBWidgetEvent &ev);
   virtual void OnResized(int old_w, int old_h);

private:
    /// internal method to create the widgets in the assembly
    void GenerateGrid();
    /// internal method to adjust the row and column sizes based on new geometry
    void ResizeGrid();
    /// internal utility method to create a row of widgets.
    virtual void AddGridRow( int rownum );

    int rows_;
    int columns_;
    int margin_;
    int rowHeight_;
    int columnWidth_;
    int addCount_;

};

}
