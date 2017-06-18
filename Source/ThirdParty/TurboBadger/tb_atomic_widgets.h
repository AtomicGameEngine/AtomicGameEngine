// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================
//
// Copyright (c) 2016-2017, THUNDERBEAST GAMES LLC All rights reserved
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
#include "tb_window.h"
#include "tb_scroll_container.h"
#include "tb_select_item.h"
#include "tb_menu_window.h"

#define UIPROMPTMESSAGEID 1
#define UIPROMPTEDITID 2
#define UIFINDEREDITPATHID 1
#define UIFINDERUPBUTTONID 2
#define UIFINDERBOOKBUTTONID 3
#define UIFINDERFOLDERBUTTONID 4
#define UIFINDERBOOKLISTID 5
#define UIFINDERFILELISTID 6
#define UIFINDEREDITFILEID 7
#define UIFINDEROKBUTTONID 8
#define UIFINDERCANCELBUTTONID 9

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


/// TBBarGraph a simple bargraph, plots 0.0 to 100.0 in the selected color
class TBBarGraph : public TBWidget
{
public:
    TBOBJECT_SUBCLASS(TBBarGraph, TBWidget); // For safe typecasting

    TBBarGraph();

    void SetColor ( const char * );
    void SetColor (float r, float g, float b, float a);
    const TBColor& GetColor() const { return color_; }

    virtual void OnInflate(const INFLATE_INFO &info);
    virtual void OnPaint(const PaintProps &paint_props);

    virtual void SetValueDouble(double value);
    virtual double GetValueDouble() { return m_value; }
    virtual void SetValue(int value) { SetValueDouble(value); }
    virtual int GetValue() { return (int) GetValueDouble(); }
    virtual void SetAxis(AXIS axis);
    virtual AXIS GetAxis() const { return m_axis; }
    virtual void SetMargin(unsigned margin) { m_margin = margin; }
    virtual unsigned GetMargin() const { return m_margin; }

private:
    TBColor color_;
    double m_value;
    AXIS m_axis;
    unsigned m_margin;
};


/** TBPromptWindow is a window for requesting a string. */
class TBPromptWindow : public TBWindow, private TBWidgetListener
{
public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(TBPromptWindow, TBWindow);
    TBPromptWindow(TBWidget *target, TBID id);
    virtual ~TBPromptWindow();
    bool Show(const char *title, const char *message,
          const char *preset = nullptr, int dimmer = 0,
          int width = 0, int height = 0);
    virtual TBWidget *GetEventDestination() { return m_target.Get(); }
    virtual bool OnEvent(const TBWidgetEvent &ev);
    virtual void OnDie();

private:
    // TBWidgetListener
    virtual void OnWidgetDelete(TBWidget *widget);
    virtual bool OnWidgetDying(TBWidget *widget);
    TBWidgetSafePointer m_dimmer;
    TBWidgetSafePointer m_target;
};

// file, path finder
class TBFinderWindow : public TBWindow, private TBWidgetListener
{
public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(TBFinderWindow, TBWindow);
    TBFinderWindow(TBWidget *target, TBID id);
    virtual ~TBFinderWindow();
    bool Show(const char *title,
          const char *preset = nullptr, int dimmer = 0,
          int width = 0, int height = 0);
    virtual TBWidget *GetEventDestination() { return m_target.Get(); }
    virtual bool OnEvent(const TBWidgetEvent &ev);
    virtual void OnDie();

private:
    // TBWidgetListener
    virtual void OnWidgetDelete(TBWidget *widget);
    virtual bool OnWidgetDying(TBWidget *widget);
    TBWidget *FindParentList( TBWidget *widget); // utility for dealing with menus.
    TBWidgetSafePointer m_dimmer;
    TBWidgetSafePointer m_target;
    TBWidget *rightMenuParent;  // information for context menus
    TBWidget *rightMenuChild;

};


/** TBSelectDropdown shows a button that opens a popup with a TBSelectList with items
    provided by a TBSelectItemSource. */

class TBPulldownMenu : public TBButton, public TBSelectItemViewer
{
public:
    // For safe typecasting
    TBOBJECT_SUBCLASS(TBPulldownMenu, TBButton);

    TBPulldownMenu();
    ~TBPulldownMenu();

    /** Get the default item source for this widget. This source can be used to add
        items of type TBGenericStringItem to this widget. */
    TBGenericStringItemSource *GetDefaultSource() { return &m_default_source; }

    /** Save the selected item. Transfers value from menu to button parent */
    virtual void SetValue(int value);
    virtual int GetValue() { return m_value; }

    /** Save the selected item TBID. Transfers value from menu to button parent */
    void SetValueID(TBID value) { m_valueid = value; }
    TBID GetValueID() { return m_valueid; }

    /** Get the ID of the selected item, or 0 if there is no item selected. */
    TBID GetSelectedItemID();

    /** Open the window if the model has items. */
    void OpenWindow();

    /** Close the window if it is open. */
    void CloseWindow();

    /** Return the menu window if it's open, or nullptr. */
    TBMenuWindow *GetMenuIfOpen() const;

    virtual void OnInflate(const INFLATE_INFO &info);
    virtual bool OnEvent(const TBWidgetEvent &ev);

    // TBSelectItemViewer 
    virtual void OnSourceChanged();
    virtual void OnItemChanged(int index) {}
    virtual void OnItemAdded(int index) {}
    virtual void OnItemRemoved(int index) {}
    virtual void OnAllItemsRemoved() {}
    
    virtual bool OnWidgetInvokeEvent(TBWidget *widget, const TBWidgetEvent &ev) { return false; } 

protected:

    void InvokeModifiedEvent(TBID entryid);

    TBGenericStringItemSource m_default_source;
    TBWidgetSafePointer m_window_pointer; ///< Points to the dropdown window if opened
    int m_value; /// which list item has been selected
    TBID m_valueid; /// the TBID of the list item that was selected
};




}; // namespace tb

#endif // TB_ATOMIC_WIDGETS_H
