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

#include <TurboBadger/tb_widgets.h>
#include <TurboBadger/tb_widgets_common.h>
#include <TurboBadger/tb_layout.h>

#include "UI.h"
#include "UIEvents.h"
#include "UIWidget.h"
#include "UILayout.h"
#include "UIButtonGrid.h"

using namespace tb;

namespace Atomic
{

UIButtonGrid::UIButtonGrid(Context* context, int numRows, int numCols, int margin, bool createWidget ) 
    : UIWidget(context, false),
    m_rows(numRows),
    m_cols(numCols),
    m_margin(margin),
    m_rowHeight(0),
    m_colWidth(0),
    m_count(0)
{
    if (createWidget)
    {
        widget_ = new TBLayout(AXIS_Y); // build upon a stock TBLayout
        widget_->SetDelegate(this);
        widget_->SetGravity(WIDGET_GRAVITY_ALL);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
        GenerateGrid();
    }
}

UIButtonGrid::~UIButtonGrid()
{
}

void UIButtonGrid::SetGridText (int row, int col, String str)
{
    if (!widget_)
        return;
       
    TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row
    if (lo0) 
    {
        TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(col);  // find col
        if (b0)
        {
           b0->SetText( str.CString() );
        } 
    }
}

int UIButtonGrid::AddGridText ( String str ) /// add strings starting at 0,0 and filling out cols, then next row, returns count
{
    int row = 0;
    int col = 0;
    if ( m_count > ( m_rows * m_cols) ) return m_count; // dont write past the end of the grid
    row = m_count / m_cols;
    col = m_count % m_cols;
    SetGridText ( row, col, str );
    m_count++;
    return m_count-1;
}

int UIButtonGrid::GetNumRows() const /// returns number of rows that were programmed
{
    return m_rows;
}

int UIButtonGrid::GetNumCols() const  /// returns number of cols that were programmed
{
    return m_cols;
}

int UIButtonGrid::GetRowHeight() const /// returns current row height
{
    return m_rowHeight;
}

int UIButtonGrid::GetColWidth() const /// returns current col width
{
    return m_colWidth;
}

int UIButtonGrid::GetMargin() const  /// returns current margin value
{
    return m_margin;
}

String UIButtonGrid::GetGridId( int row, int col ) 
{
    return String( 'A' + row ) + String(col); // generate spreadsheet style id
}

String UIButtonGrid::GetGridText(int row, int col)
{
    if (!widget_)
        return "";
    
    TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row
    if (lo0) 
    {
        TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(col);  // find col
        if (b0)
        {
            TBStr foo;
            if ( b0->GetText( foo ) )
                return foo.CStr();
        } 
    }
    return "";
}

String UIButtonGrid::AtGridText( int count ) /// returns text at count
{
    int row = 0;
    int col = 0;
    row = count / m_cols;
    col = count % m_cols;
    return GetGridText( row, col );
}

UIWidget* UIButtonGrid::GetGridWidget(int row, int col)
{
    if (!widget_)
        return NULL;

    TBWidget *mywidget = NULL;
    TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row); // find row
    if (lo0) 
    {
        mywidget = lo0->GetChildFromIndex(col);  // find col
    }
    if ( mywidget )
    {
        UI* ui = GetSubsystem<UI>();
        return ui->WrapWidget(mywidget);
    }
   return NULL; 
}

UIWidget* UIButtonGrid::AtGridWidget( int count ) /// returns widget at count
{
    int row = 0;
    int col = 0;
    row = count / m_cols;
    col = count % m_cols;
    return GetGridWidget ( row, col );
}

void UIButtonGrid::GenerateGrid()
{
    if ( widget_ && widget_->numChildren() == 0 )  // build once.
    {
        ((TBLayout *)widget_)->SetSpacing(m_margin);
        for (int nn=0; nn<m_rows; nn++ )
            AddGridRow( nn );
    }
}

void UIButtonGrid::AddGridRow( int rownum )
{
    TBLayout *lo0 = new TBLayout(); // make a new layout
    lo0->SetID( TBID (rownum) );
    lo0->SetLayoutConfig ( "XACAC" ); // do config + (spacing) margin
    lo0->SetSpacing(m_margin);
    int cc = 0;
    for ( cc=0; cc<m_cols; cc++) // stuff new button ( with preferred size, new id ) in.
    {
        LayoutParams *lp0 = new LayoutParams();
        lp0->SetWidth( m_colWidth <= 0 ? 1 : m_colWidth);
        lp0->SetHeight( m_rowHeight <= 0 ? 1 : m_rowHeight);
        TBButton *b0 = new TBButton();
        b0->SetLayoutParams(*lp0);
        b0->SetSqueezable(true);
        TBStr myid;
        myid.SetFormatted ( "%c%d", 'A' + rownum, cc+1 );
        b0->SetID( TBID (myid) );
        lo0->AddChild ( b0 );
    }
    widget_->AddChild ( lo0 );
}

void UIButtonGrid::DisableEmptyButtons() /// will disable buttons that havent set any text, and enable those with text.
{
    int row = 0;
    int col = 0;
    for ( row=0; row<m_rows; row++ )
    {
        TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row layout
        if (lo0) 
        {
            for ( col=0; col<m_cols; col++ )
            {
                TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(col);  // find col button
                if (b0)
                {
                    TBStr foo;
                    if ( b0->GetText( foo ) )
                    {
                        b0->SetState(WIDGET_STATE_DISABLED, foo.IsEmpty() );
                    }
                }
            }
        }
    }  
}


void UIButtonGrid::ResizeGrid()
{
    if ( m_rows == 0 || m_cols == 0 ) return; // dont do bad maths.

    TBRect myrect = widget_->GetRect();
    m_rowHeight = (int)(  (myrect.h - (m_margin * m_rows )) / m_rows );
    m_colWidth = (int)( (myrect.w -( m_margin * m_cols )) / m_cols );

    if ( m_rowHeight <= 1) m_rowHeight = 1;
    if ( m_colWidth <= 1)  m_colWidth = 1;

    int row = 0;
    int col = 0;
    for ( row=0; row<m_rows; row++ )
    {
        TBLayout *lo0 = (TBLayout *)widget_->GetChildFromIndex(row);  // find row layout
        if (lo0) 
        {
            for ( col=0; col<m_cols; col++ )
            {
                TBButton *b0 = (TBButton *)lo0->GetChildFromIndex(col);  // find col button
                if (b0)
                {
                    LayoutParams *lp1 = new LayoutParams(); // replace with new calced values
                    lp1->SetWidth(m_colWidth);
                    lp1->SetHeight(m_rowHeight);
                    b0->SetLayoutParams(*lp1);
                }
            }
        }
    }
}

void UIButtonGrid::OnResized(int old_w, int old_h) 
{
    ResizeGrid();
}

bool UIButtonGrid::OnEvent(const tb::TBWidgetEvent &ev)
{
    return UIWidget::OnEvent(ev);
}

}
