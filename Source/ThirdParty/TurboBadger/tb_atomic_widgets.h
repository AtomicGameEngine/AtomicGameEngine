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
#ifndef TB_ATOMIC_WIDGETS_H
#define TB_ATOMIC_WIDGETS_H

#include "tb_widgets.h"
#include "tb_layout.h"
#include "tb_msg.h"
#include "tb_widgets_listener.h"
#include "tb_widgets_common.h"

namespace tb {

// fruxo recommends : Subclass TBWidget and override OnPaint. From
// there you can paint solid colors using g_tb_skin->PaintRectFill.
class TBColorWidget : public TBWidget
{
public:
    TBOBJECT_SUBCLASS(TBColorWidget, TBWidget);    // For safe typecasting

    TBColorWidget();

    void SetColor ( const char * );
    void SetColor (float r, float g, float b, float a);

    void SetAlpha ( float );

    const TBColor& GetColor() const { return color_; }
    float GetAlpha() const { return alpha_; }

    virtual void OnInflate(const INFLATE_INFO &info);
    virtual void OnPaint(const PaintProps &paint_props);

private:

	TBColor color_;
    float alpha_;
};

class TBColorWheel : public TBWidget
{
public:
    TBOBJECT_SUBCLASS(TBColorWheel, TBWidget);    // For safe typecasting

    TBColorWheel();

    virtual void OnInflate(const INFLATE_INFO &info);
    virtual void OnPaint(const PaintProps &paint_props);
    virtual bool OnEvent(const TBWidgetEvent &ev);

    float GetHue() const { return hue_; }
    float GetSaturation() const { return saturation_; }
    void SetHueSaturation ( float hue, float saturation );
    
    void SetMarkerX ( int );
    void SetMarkerY( int );
    void SetMarkerColor ( const char * );
    
private:

    void CalcHueSaturation ( int, int ); // maths.

    int markerx_;
    int markery_; // where we clicked, put a box there
	TBColor markercolor_; // what color box, default = black
    float hue_;   // varies with the angle
    float saturation_; // varies with the radius.

};

class TBEventBlocker : public TBWidgetListener
{

public:

    TBEventBlocker() : TBWidgetListener() { }
    
    /** Called when a event is about to be invoked on a widget. This make it possible
        to intercept events before they are handled, and block it (by returning true --like here ).
        Note, if returning true, other global listeners will still also be notified. */
    virtual bool OnWidgetInvokeEvent(TBWidget *widget, const TBWidgetEvent &ev) { return true; } 
    
};


}; // namespace tb

#endif // TB_ATOMIC_WIDGETS_H
