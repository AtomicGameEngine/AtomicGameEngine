// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================
//
// Copyright (c) 2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include "tb_widgets_reader.h"
#include "tb_widgets_common.h"
#include "tb_node_tree.h"
#include "tb_system.h"
#include "tb_atomic_widgets.h"
#include <math.h>

namespace tb {

// == TBColorWidget =======================================

TBColorWidget::TBColorWidget() : color_(), alpha_ ( 1.0f)
{
}

void TBColorWidget::SetColor ( const char *name )
{
    if ( name )
        color_.SetFromString(name, strlen(name));
     
    InvalidateSkinStates();
    Invalidate();
}

void TBColorWidget::SetColor(float r, float g, float b, float a)
{
    color_.Set(TBColor(r, g, b, a));

    InvalidateSkinStates();
    Invalidate();
}

void TBColorWidget::SetAlpha ( float value )
{
    if ( value < 0.0 || value > 1.0 ) 
    {
        alpha_ = 1.0;
        return;
    }
    
    alpha_ = value; 

    InvalidateSkinStates();
    Invalidate();
}

void TBColorWidget::OnPaint(const PaintProps &paint_props)
{
    TBRect local_rect = GetRect();
    local_rect.x = 0;
    local_rect.y = 0;
    float old_opacity = g_renderer->GetOpacity();
    g_renderer->SetOpacity(alpha_);
    g_renderer->DrawRectFill(local_rect, color_);
    g_renderer->SetOpacity(old_opacity);
}

void TBColorWidget::OnInflate(const INFLATE_INFO &info)
{
    if (const char *colr = info.node->GetValueString("color", nullptr))
        SetColor(colr);
    TBWidget::OnInflate(info);
}

TB_WIDGET_FACTORY(TBColorWidget, TBValue::TYPE_NULL, WIDGET_Z_TOP) {}


// == TBColorWheel =======================================

TBColorWheel::TBColorWheel() :
    markerx_(128),
    markery_(128),
    markercolor_(),
    hue_(0.0),
    saturation_(0.0)
{
}

void TBColorWheel::OnPaint(const PaintProps &paint_props)
{
    TBWidget::OnPaint(paint_props);  // draw the widget stuff

    TBRect local_rect ( 0,0,4,4 ); // AND draw a marker where we clicked.
    local_rect.x = markerx_ - 2;
    local_rect.y = markery_ - 2;
    g_renderer->DrawRect( local_rect, markercolor_);
    local_rect.x -= 1;
    local_rect.y -= 1;
    local_rect.w += 2;
    local_rect.h += 2;
    g_renderer->DrawRect( local_rect, markercolor_);  // draw double box
    
}

bool TBColorWheel::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.target == this && ev.type == EVENT_TYPE_CLICK)
    {
         SetMarkerX ( ev.target_x );
         SetMarkerY ( ev.target_y );
         CalcHueSaturation( markerx_, markery_ );
         TBWidgetEvent ev(EVENT_TYPE_CHANGED); 
         InvokeEvent(ev);
    }
    return TBWidget::OnEvent(ev);
}

void TBColorWheel::SetHueSaturation ( float hue, float saturation )
{

    // suppose to set the marker position to match HS here

    hue_ = hue * 360.0;
    saturation_ = saturation * 128.0;
    
    Invalidate();
}

void TBColorWheel::CalcHueSaturation ( int rawx, int rawy )
{
    TBRect rect = GetRect(); 
    int centerx = rect.w / 2;
    int centery = rect.h / 2;

    float X1 = rawx;
    float Y1 = rawy;
    float X2 = centerx;
    float Y2 = centery; 
    float angle = 0.0;
    float xd = X2-X1;
    float yd = Y2-Y1;
    float dx = sqrt(xd * xd + yd * yd);

    // angle in degrees
    angle = atan2(Y2 - Y1, X2 - X1) * 180 / 3.14159265358979323846;
    if (angle < 0) angle += 360.0;

    // if the distance > 128, can we calculate the line point at 128 and set the marker there?

    if( dx > 128.0 ) dx = 128.0;  // limit value
    
    saturation_ = dx;
    hue_ = angle;
}

void TBColorWheel::SetMarkerX ( int value )
{
    markerx_ = value;
}

void TBColorWheel::SetMarkerY ( int value )
{
    markery_ = value;
}

void TBColorWheel::SetMarkerColor ( const char *name )
{
    if ( name )
        markercolor_.SetFromString(name, strlen(name));
     
    Invalidate();
}

void TBColorWheel::OnInflate(const INFLATE_INFO &info)
{
    if (const char *colr = info.node->GetValueString("color", nullptr))
        SetMarkerColor(colr);
    TBWidget::OnInflate(info);
}

TB_WIDGET_FACTORY(TBColorWheel, TBValue::TYPE_FLOAT, WIDGET_Z_TOP) {}


// == TBBarGraph =======================================

TBBarGraph::TBBarGraph() : color_(255,255,255,255), m_value (0.0), m_axis(AXIS_X), m_margin(0)
{
    SetSkinBg(TBIDC("background_solid"), WIDGET_INVOKE_INFO_NO_CALLBACKS);
}

void TBBarGraph::SetColor ( const char *name )
{
    if ( name )
        color_.SetFromString(name, strlen(name));
     
    InvalidateSkinStates();
    Invalidate();
}

void TBBarGraph::SetColor(float r, float g, float b, float a)
{
    color_.Set(TBColor(r, g, b, a));

    InvalidateSkinStates();
    Invalidate();
}

void TBBarGraph::OnPaint(const PaintProps &paint_props)
{
    TBRect local_rect = GetRect();
    local_rect.x = 0;
    local_rect.y = 0;
    
    if ( m_axis == AXIS_X ) // horizontal bar
    {
        double w1 = (double)local_rect.w * ( m_value / 100.0 );
        local_rect.w = (int)w1;
        if ( m_margin > 0 && m_margin < (local_rect.h/2)-2)
        {
            local_rect.h -= (m_margin *2);
            local_rect.y += m_margin;
        }
    }
    else if ( m_axis == AXIS_Y ) // vertical bar
    {
        double h1 = (double)local_rect.h * ( m_value / 100.0 );
        local_rect.y = local_rect.h - (int)h1;
        local_rect.h = (int)h1;
        if ( m_margin > 0 && m_margin < (local_rect.w/2)-2 )
        {
            local_rect.w -= (m_margin*2);
            local_rect.x += m_margin;
        }
    }
    g_renderer->DrawRectFill(local_rect, color_);
}

void TBBarGraph::OnInflate(const INFLATE_INFO &info)
{
    if (const char *colr = info.node->GetValueString("color", nullptr))
        SetColor(colr);
    if ( const char *axis = info.node->GetValueString("axis", "x") )
        SetAxis(*axis == 'x' ? AXIS_X : AXIS_Y);
    if (info.sync_type == TBValue::TYPE_FLOAT)
        SetValueDouble(info.node->GetValueFloat("value", 0));
    SetMargin( (unsigned)info.node->GetValueInt("margin", 0 ) );
    TBWidget::OnInflate(info);
}

void TBBarGraph::SetValueDouble(double value)
{
    value = CLAMP(value, 0.0, 100.0);
    if (value == m_value)
        return;
    m_value = value;

    InvalidateSkinStates();
    Invalidate();
}

void TBBarGraph::SetAxis(AXIS axis) 
{ 
    m_axis = axis; 
    InvalidateSkinStates();
    Invalidate();
}

TB_WIDGET_FACTORY(TBBarGraph, TBValue::TYPE_FLOAT, WIDGET_Z_TOP) {}

}; // namespace tb
