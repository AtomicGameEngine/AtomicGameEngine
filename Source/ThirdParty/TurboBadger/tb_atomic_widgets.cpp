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

#include "tb_widgets_reader.h"
#include "tb_widgets_common.h"
#include "tb_node_tree.h"
#include "tb_system.h"
#include "tb_atomic_widgets.h"
#include "tb_editfield.h"
#include "tb_menu_window.h"
#include "tb_select.h"
#include "tb_tab_container.h"
#include "image/tb_image_widget.h"

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


// == TBPromptWindow =======================================


TBPromptWindow::TBPromptWindow(TBWidget *target, TBID id)
    : m_target(target)
{
    TBWidgetListener::AddGlobalListener(this);
    SetID(id);
}

TBPromptWindow::~TBPromptWindow()
{
    TBWidgetListener::RemoveGlobalListener(this);
    if (TBWidget *dimmer = m_dimmer.Get())
    {
        dimmer->GetParent()->RemoveChild(dimmer);
        delete dimmer;
    }
}

bool TBPromptWindow::Show(const char *title, const char *message,
    const char *preset, int dimmer,
    int width, int height)
{
    TBWidget *target = m_target.Get();
    if (!target)
        return false;

    TBWidget *root = target->GetParentRoot();

    const char *source = "TBLayout: axis: y, distribution: gravity, position: left\n"
    "	TBLayout: axis: y, distribution: gravity, distribution-position: left\n"
    "		TBTextField: id: 1, gravity: left right\n"
    "			font: size: 14dp\n"
    "		TBEditField: id: 2, multiline: 0, styling: 0, adapt-to-content: 0, gravity: left right\n"
     "			font: size: 14dp\n"
   "	TBSeparator: gravity: left right\n"
    "	TBLayout: distribution: gravity\n"
    "		TBButton: text: \"  OK  \", id: \"TBPromptWindow.ok\"\n"
    "			font: size: 14dp\n"
    "		TBButton: text: \"Cancel\", id: \"TBPromptWindow.cancel\"\n"
    "			font: size: 14dp\n";

    if (!g_widgets_reader->LoadData(GetContentRoot(), source))
        return false;

    SetText(title);

    TBTextField *editfield = GetWidgetByIDAndType<TBTextField>(UIPROMPTMESSAGEID);
    editfield->SetText(message);
    editfield->SetSkinBg("");

    TBEditField *stringfield = GetWidgetByIDAndType<TBEditField>(UIPROMPTEDITID);
    if (preset) 
        stringfield->SetText(preset);

    TBRect rect;

    // Size to fit content. This will use the default size of the textfield.
    if (width == 0 || height == 0)
    {
        ResizeToFitContent();
        rect = GetRect();
    }
    else
    {
        SetSize(width, height);
        rect = GetRect();
    }

    // Create background dimmer
    if (dimmer != 0)
    {
        if (TBDimmer *dimmer = new TBDimmer)
        {
            root->AddChild(dimmer);
            m_dimmer.Set(dimmer);
        }
    }

    // Center and size to the new height
    TBRect bounds(0, 0, root->GetRect().w,  root->GetRect().h);
    SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));
    root->AddChild(this);
    return true;
}

bool TBPromptWindow::OnEvent(const TBWidgetEvent &ev)
{
    if (ev.type == EVENT_TYPE_CLICK && ev.target->IsOfType<TBButton>())
    {
        TBWidgetSafePointer this_widget(this);

        // Invoke the click on the target
        TBWidgetEvent target_ev(EVENT_TYPE_CLICK);
        target_ev.ref_id = ev.target->GetID();
        InvokeEvent(target_ev);

        // If target got deleted, close
        if (this_widget.Get())
            Close();

        return true;
    }
    else if (ev.type == EVENT_TYPE_KEY_DOWN && ev.special_key == TB_KEY_ESC)
    {
        TBWidgetEvent click_ev(EVENT_TYPE_CLICK);
        m_close_button.InvokeEvent(click_ev);
        return true;
    }
    return TBWindow::OnEvent(ev);
}

void TBPromptWindow::OnDie()
{
    if (TBWidget *dimmer = m_dimmer.Get())
        dimmer->Die();
}

void TBPromptWindow::OnWidgetDelete(TBWidget *widget)
{
    // If the target widget is deleted, close!
    if (!m_target.Get())
        Close();
}

bool TBPromptWindow::OnWidgetDying(TBWidget *widget)
{
    // If the target widget or an ancestor of it is dying, close!
    if (widget == m_target.Get() || widget->IsAncestorOf(m_target.Get()))
        Close();
    return false;
}


// == TBFinderWindow =======================================


TBFinderWindow::TBFinderWindow(TBWidget *target, TBID id)
    : m_target(target),
    rightMenuParent(NULL),
    rightMenuChild(NULL)
{
    TBWidgetListener::AddGlobalListener(this);
    SetID(id);
}

TBFinderWindow::~TBFinderWindow()
{
    TBWidgetListener::RemoveGlobalListener(this);
    if (TBWidget *dimmer = m_dimmer.Get())
    {
        dimmer->GetParent()->RemoveChild(dimmer);
        delete dimmer;
    }
    rightMenuParent=NULL;
    rightMenuChild=NULL;
}


bool TBFinderWindow::Show(const char *title,
    const char *preset, int dimmer,
    int width, int height)
{
    TBWidget *target = m_target.Get();
    if (!target)
        return false;

    TBWidget *root = target->GetParentRoot();


    const char *source = 
    "TBLayout: axis: y, size: available, position: gravity, distribution: gravity\n"
    "	lp: min-width: 512dp, min-height: 500dp\n"
    "	TBLayout: distribution: gravity, distribution-position: left\n"
    "		TBEditField: id: 1, multiline: 0, styling: 0, adapt-to-content: 0, gravity: left right\n"
    "			tooltip current folder location\n"
    "			font: size: 14dp\n"
    "		TBButton\n"
    "			lp: height: 28dp, width: 28dp\n"
    "			skin TBButton.uniformflat\n"
    "			TBSkinImage: skin: FolderUp, id: up_image\n"
    "			id 2\n"
    "			tooltip Go up one folder\n"
    "		TBWidget\n"
    "			lp: height: 28dp, width: 28dp\n"
    "		TBButton\n"
    "			lp: height: 28dp, width: 28dp\n"
    "			skin TBButton.uniformflat\n"
    "			TBSkinImage: skin: BookmarkIcon, id: book_image\n"
    "			id 3\n"
    "			tooltip Bookmark current folder\n"
    "		TBButton\n"
    "			lp: height: 28dp, width: 28dp\n"
    "			skin TBButton.uniformflat\n"
    "			TBSkinImage: skin: FolderAdd, id: create_image\n"
    "			id 4\n"
    "			tooltip Create a new folder \n"
    "	TBLayout: distribution: gravity, distribution-position: left\n"
    "		TBLayout: axis: x, distribution: gravity\n"
    "			TBSelectList: id: 5, gravity: all\n"
    "				lp: max-width: 160dp, min-width: 160dp\n"
    "				font: size: 14dp\n"
    "			TBSelectList: id: 6, gravity: all\n"
    "				font: size: 14dp\n"
    "	TBLayout: distribution: gravity\n"
    "		TBEditField: id: 7, multiline: 0, styling: 0, adapt-to-content: 0, gravity: left right\n"
    "			tooltip name of the wanted file\n"
    "			font: size: 14dp\n"
    "	TBLayout: distribution: gravity\n"
    "		TBButton: text: \"  OK  \", id: 8\n"
    "			font: size: 14dp\n"
    "		TBButton: text: \"Cancel\", id: 9\n"
    "			font: size: 14dp\n";

    if (!g_widgets_reader->LoadData(GetContentRoot(), source))
        return false;

    SetText(title);
    TBRect rect;

    // Size to fit content. This will use the default size of the textfield.
    if (width == 0 || height == 0)
    {
        ResizeToFitContent();
        rect = GetRect();
    }
    else
    {
        SetSize(width, height);
        rect = GetRect();
    }

    // Create background dimmer
    if (dimmer != 0)
    {
        if (TBDimmer *dimmer = new TBDimmer)
        {
            root->AddChild(dimmer);
            m_dimmer.Set(dimmer);
        }
    }

    // Center and size to the new height
    TBRect bounds(0, 0, root->GetRect().w,  root->GetRect().h);
    SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));
    root->AddChild(this);
    return true;
}

bool TBFinderWindow::OnEvent(const TBWidgetEvent &ev)
{

    if (ev.type == EVENT_TYPE_CLICK )
    {
        if (  ev.target->IsOfType<TBButton>())
        {
            TBWidgetSafePointer this_widget(this);

            // Invoke the click on the target
            TBWidgetEvent target_ev(EVENT_TYPE_CLICK);
            target_ev.ref_id = ev.target->GetID();
            InvokeEvent(target_ev);

         // these are internal buttons that do not close the finder window!
            bool isbuttons = (ev.target->GetID() == UIFINDERUPBUTTONID
                || ev.target->GetID() == UIFINDERBOOKBUTTONID
                || ev.target->GetID() == UIFINDERFOLDERBUTTONID );
            // If target got deleted, close
            if (this_widget.Get() && !isbuttons )
                Close();
            return true;
        }
        else if ( ev.target->GetID() == TBIDC("popupmenu"))
        {
            if (ev.ref_id == TBIDC("delete"))
            {
                // send EVENT_TYPE_CUSTOM, were gonna used cached information to send info how we got here
                if(rightMenuParent)
                {
                    TBWidgetEvent custom_ev(EVENT_TYPE_CUSTOM);
                    custom_ev.target = rightMenuParent;  // were want to operate on this list
                    custom_ev.ref_id = rightMenuChild?rightMenuChild->GetID():ev.ref_id; // on this entry 
                    custom_ev.special_key = TB_KEY_DELETE;  // and what we wanna do to it
                    rightMenuParent->InvokeEvent(custom_ev); // forward to delegate
                    rightMenuChild = NULL; // clear the cached values
                    rightMenuParent = NULL;
                }
            }
            else
                return false;
            return true;
        }
    }
    else if (ev.type == EVENT_TYPE_KEY_DOWN && ev.special_key == TB_KEY_ESC)
    {
        TBWidgetEvent click_ev(EVENT_TYPE_CLICK);
        m_close_button.InvokeEvent(click_ev);
        return true;
    }
    else if (ev.type == EVENT_TYPE_CONTEXT_MENU || ev.type == EVENT_TYPE_RIGHT_POINTER_UP ) // want to embed popup menu on TBSelectList id: 5
    {
        rightMenuChild = ev.target; // save for later, this is where we started
        rightMenuParent = FindParentList(ev.target);  // save for later, omg why is this so hard!
        if ( rightMenuParent && rightMenuParent->GetID() == UIFINDERBOOKLISTID ) // if we clicked in bookmark list take action!
        {
            TBPoint pos_in_root(ev.target_x, ev.target_y);
            if (TBMenuWindow *menu = new TBMenuWindow(rightMenuParent, TBIDC("popupmenu")))
            {
                TBGenericStringItemSource *source = menu->GetList()->GetDefaultSource();
                source->AddItem(new TBGenericStringItem("delete", TBIDC("delete")));
                menu->Show(source, TBPopupAlignment(pos_in_root), -1);
            }
            return true;
        }
    }

    return TBWindow::OnEvent(ev);
}

void TBFinderWindow::OnDie()
{
    if (TBWidget *dimmer = m_dimmer.Get())
        dimmer->Die();
}

void TBFinderWindow::OnWidgetDelete(TBWidget *widget)
{
    // If the target widget is deleted, close!
    if (!m_target.Get())
        Close();
}

bool TBFinderWindow::OnWidgetDying(TBWidget *widget)
{
    // If the target widget or an ancestor of it is dying, close!
    if (widget == m_target.Get() || widget->IsAncestorOf(m_target.Get()))
        Close();
    return false;
}

TBWidget *TBFinderWindow::FindParentList( TBWidget *widget) // utility for dealing with menus.
{
    TBWidget *tmp = widget;
    while (tmp)
    {
        if ( tmp->IsOfType<TBSelectList>() ) return tmp;
        tmp = tmp->GetParent();
    }
    return NULL;
}

// == TBPulldownMenu ==========================================

TBPulldownMenu::TBPulldownMenu()
    : m_value(-1)
{
    SetSource(&m_default_source);
    SetSkinBg(TBIDC("TBSelectDropdown"), WIDGET_INVOKE_INFO_NO_CALLBACKS);
}

TBPulldownMenu::~TBPulldownMenu()
{
    SetSource(nullptr);
    CloseWindow();
}

void TBPulldownMenu::OnSourceChanged()
{
    m_value = -1;
    m_valueid = TBID();
    if (m_source && m_source->GetNumItems())
        SetValue(0);
}

void TBPulldownMenu::SetValue(int value)
{
    if (!m_source)
        return;
    m_value = value;
    m_valueid = GetSelectedItemID();
    InvokeModifiedEvent( m_valueid );
}

TBID TBPulldownMenu::GetSelectedItemID()
{
    if (m_source && m_value >= 0 && m_value < m_source->GetNumItems())
        return m_source->GetItemID(m_value);
    return TBID();
}

void TBPulldownMenu::InvokeModifiedEvent( TBID entryid )
{
    TBWidgetEvent ev( EVENT_TYPE_CHANGED);
    // TBIDC does not register the TBID with the UI system, so do it this way
    ev.target = this;      // who am I
    ev.ref_id = entryid;   // id of whom we clicked
    TBWidget::OnEvent(ev); // forward to delegate
}

void TBPulldownMenu::OpenWindow()
{
    if (!m_source || !m_source->GetNumItems() || m_window_pointer.Get())
        return;

    if (TBMenuWindow *window = new TBMenuWindow(this, TBIDC("TBPulldownMenu.menu")))
    {
        m_window_pointer.Set(window);
        window->SetSkinBg(TBIDC("TBSelectDropdown.window"));
        window->Show(m_source, TBPopupAlignment());
    }
}

void TBPulldownMenu::CloseWindow()
{
    if (TBMenuWindow *window = GetMenuIfOpen())
        window->Close();
}

TBMenuWindow *TBPulldownMenu::GetMenuIfOpen() const
{
    return TBSafeCast<TBMenuWindow>(m_window_pointer.Get());
}

bool TBPulldownMenu::OnEvent(const TBWidgetEvent &ev)
{
    if ( ev.target->IsOfType<TBButton>() && ev.type == EVENT_TYPE_CLICK)
    {
        // Open the menu, or set the value and close it if already open (this will
        // happen when clicking by keyboard since that will call click on this button)
        if (TBMenuWindow *menu_window = GetMenuIfOpen())
        {
            TBWidgetSafePointer tmp(this);
            int value = menu_window->GetList()->GetValue();
            menu_window->Die();
            if (tmp.Get())
                SetValue(value);
        }
        else
            OpenWindow();
        return true;
    }
    else if (ev.target->GetID() == TBIDC("TBPulldownMenu.menu") && ev.type == EVENT_TYPE_CLICK )
    {
        if (TBMenuWindow *menu_window = GetMenuIfOpen())
            SetValue(menu_window->GetList()->GetValue());
        return true;
    }
    else if (ev.target == this && m_source && ev.IsKeyEvent())
    {
        if (TBMenuWindow *menu_window = GetMenuIfOpen())
        {
            // Redirect the key strokes to the list
            TBWidgetEvent redirected_ev(ev);
            return menu_window->GetList()->InvokeEvent(redirected_ev);
        }
    }
    return false;
}

TB_WIDGET_FACTORY( TBPulldownMenu, TBValue::TYPE_INT, WIDGET_Z_TOP) {}

extern void ReadItems(TBNode *node, TBGenericStringItemSource *target_source);  // blind function that lives in tb_widgets_reader.cpp

void TBPulldownMenu::OnInflate(const INFLATE_INFO &info)
{
    // Read items (if there is any) into the default source
    ReadItems(info.node, GetDefaultSource());
    TBWidget::OnInflate(info);
}

// == TBDockWindow ==========================================

TBDockWindow::TBDockWindow( TBStr title, TBWidget *contentptr, int minwidth, int minheight ) : TBWindow()
{
    SetID(TBID(title));
    m_mover.AddChild(&m_redock_button);
    m_redock_button.SetSkinBg(TBIDC("TBWindow.redock"));
    if ( m_redock_button.GetSkinBgElement() == NULL )
        m_redock_button.SetSkinBg(TBIDC("arrow.down"));  // fallback skin for editor
    m_redock_button.SetIsFocusable(false);
    m_redock_button.SetID(TBIDC("TBWindow.redock"));
    m_redock_button.SetVisibilility(WIDGET_VISIBILITY_INVISIBLE);

    SetText(title);

    TBStr uilayout; // undock host ui layout
    uilayout.SetFormatted ( "TBLayout: axis: y, distribution: gravity\n\tlp: min-width: %ddp, min-height: %ddp\n\tTBLayout: id: \"undocklayout\", distribution: gravity\n", minwidth, minheight );
    g_widgets_reader->LoadData(this, uilayout );
    TBLayout *lo1 = GetWidgetByIDAndType<TBLayout>(TBID("undocklayout"));
    lo1->AddChild(contentptr); // jam it into the window before the pointer gets stale
    ResizeToFitContent();
} 

TBDockWindow::~TBDockWindow()
{
    if (m_resizer.GetParent()) RemoveChild(&m_resizer);
    if (m_mover.GetParent()) RemoveChild(&m_mover);
    if (m_close_button.GetParent()) m_mover.RemoveChild(&m_close_button);
    if (m_redock_button.GetParent()) m_mover.RemoveChild(&m_redock_button);
    m_mover.RemoveChild(&m_textfield);
}

void TBDockWindow::SetDockOrigin( TBID dockid )
{
    m_dockid = dockid;
    if ( m_dockid > 0 ) //enable/show the (re)dock button is a return id is specified
    {
        m_redock_button.SetVisibilility(WIDGET_VISIBILITY_VISIBLE);
    }
}

TBWidget *TBDockWindow::GetEventDestination() 
{ 
    if ( m_dockid > 0 ) // if the origin is specified, send events back to there while the content is undocked
    {                   // so the original event handlers will still function.
        return GetParentRoot(true)->GetWidgetByID(m_dockid);
    }
    return GetParent();
}

void TBDockWindow::Show( TBWidget *host, int xpos, int ypos )
{
    if ( host )
    {
        host->AddChild(this);
        SetPosition( TBPoint( xpos, ypos) );
    }
}

TBWidget *TBDockWindow::GetDockContent()
{
    TBLayout *lo1 = GetWidgetByIDAndType<TBLayout>(TBID("undocklayout"));
    if ( lo1 )
        return lo1->GetChildFromIndex(0);
}

bool TBDockWindow::HasDockContent()
{
    return ( GetDockContent() != NULL );
}

void TBDockWindow::Redock ()
{
    if ( m_dockid > 0 ) // see if there is a dock point id specified  
    {
        TBWidget *mytarget = GetParentRoot(true)->GetWidgetByID(m_dockid);
        if (mytarget)
            Dock (mytarget);
    }
}

void TBDockWindow::Dock ( TBWidget *target )
{
    if ( !HasDockContent() ) return; 

    if ( target ) // what kind of widget is it?
    {
        TBStr mystr;
        TBWidget *mypage = NULL;
        TBLayout *lo1 = NULL;
        TBTabContainer *tc1 = NULL;
        if ( tc1 = TBSafeCast<TBTabContainer>( target ) ) // handle TBTabContainer
        {
            mystr = GetText();
            mypage = GetDockContent();
            if (mypage)
            {
                TBButton *tbb = new TBButton();
                tbb->SetID (TBID(mystr));
                tbb->SetText(mystr);
                mypage->GetParent()->RemoveChild(mypage);
                tc1->GetContentRoot()->AddChild(mypage);
                tc1->GetTabLayout()->AddChild(tbb);
                tc1->Invalidate();
                tc1->SetCurrentPage(tc1->GetNumPages()-1);
                Close();
            }
        }
    }   
}

bool TBDockWindow::OnEvent(const TBWidgetEvent &ev)
{
   if (ev.target == &m_close_button)
    {
        if (ev.type == EVENT_TYPE_CLICK)
            Close();
        return true;
    }
   if (ev.target == &m_redock_button)
    {
        if (ev.type == EVENT_TYPE_CLICK)
            Redock();
        return true;
    }
    return TBWidget::OnEvent(ev);
}

void TBDockWindow::OnResized(int old_w, int old_h)
{
    // Apply gravity on children
    TBWidget::OnResized(old_w, old_h);
    // Manually move our own decoration children
    // FIX: Put a layout in the TBMover so we can add things there nicely.
    int title_height = GetTitleHeight();
    if ( m_axis == AXIS_Y )  // default 
    {
        m_mover.SetRect(TBRect(0, 0, GetRect().w, title_height));
        PreferredSize ps = m_resizer.GetPreferredSize();
        m_resizer.SetRect(TBRect(GetRect().w - ps.pref_w, GetRect().h - ps.pref_h, ps.pref_w, ps.pref_h));
        TBRect mover_rect = m_mover.GetPaddingRect();
        int button_size = mover_rect.h;
        m_close_button.SetRect(TBRect(mover_rect.x + mover_rect.w - button_size, mover_rect.y, button_size, button_size));
        if (m_settings & WINDOW_SETTINGS_CLOSE_BUTTON)
            mover_rect.w -= button_size;
        // add redock button to header
        m_redock_button.SetRect(TBRect(mover_rect.x + mover_rect.w - button_size, mover_rect.y, button_size, button_size));
        if (m_settings & WINDOW_SETTINGS_CLOSE_BUTTON)
            mover_rect.w -= button_size;
        m_textfield.SetRect(mover_rect);
    }
    else if ( m_axis == AXIS_X )  // rotated sideways
    {
        m_mover.SetRect(TBRect(0, 0, title_height, GetRect().h ));
        PreferredSize ps = m_resizer.GetPreferredSize();
        m_resizer.SetRect(TBRect(GetRect().w - ps.pref_w, GetRect().h - ps.pref_h, ps.pref_w, ps.pref_h));
        TBRect mover_rect = m_mover.GetPaddingRect();
        int button_size = mover_rect.w;
        m_close_button.SetRect(TBRect(mover_rect.x + 1, mover_rect.y + 1, button_size, button_size));
        if (m_settings & WINDOW_SETTINGS_CLOSE_BUTTON)
            mover_rect.y += button_size;
        m_redock_button.SetRect(TBRect(mover_rect.x + 1, mover_rect.y + 1, button_size, button_size));
        if (m_settings & WINDOW_SETTINGS_CLOSE_BUTTON)
            mover_rect.y -= button_size;
        m_textfield.SetRect(TBRect(mover_rect.x + 5, mover_rect.y + mover_rect.h - button_size, button_size - 1, button_size));
    }
}


// == MultiList Support  ==========================================


void MultiItem::AddColumn (TBStr widgettype, TBStr string, int width )
{
    TBValue *new_str = colStr_.GetArray()->AddValue();
    new_str->SetString(string.CStr(), TBValue::SET_NEW_COPY );
    TBValue *new_widget = colWidget_.GetArray()->AddValue();
    new_widget->SetString( widgettype.CStr(), TBValue::SET_NEW_COPY );
    TBValue *new_width = colWidth_.GetArray()->AddValue();
    new_width->SetInt(width); 
}

const char* MultiItem::GetColumnStr( int col ) 
{
    const char* strx = colStr_.GetArray()->GetValue(col)->GetString();
    return strx;
}

const char* MultiItem::GetColumnWidget( int col )
{
    const char* strx = colWidget_.GetArray()->GetValue(col)->GetString();
    return strx;
}

int MultiItem::GetColumnWidth( int col )
{
   return colWidth_.GetArray()->GetValue(col)->GetInt();
}

int MultiItem::GetColumnHeight()
{
   return colHeight_;
}

int MultiItem::GetNumColumns()
{
    return colStr_.GetArrayLength();
}


bool MultiItemSource::Filter(int index, const char *filter)
{
    // Override this method so we can return hits for our extra data too.

    if (TBSelectItemSource::Filter(index, filter))
        return true;

    // do this on ALL of the columns?
    return false;
}

TBWidget *MultiItemSource::CreateItemWidget(int index, TBSelectItemViewer *viewer)
{
    if (TBLayout *layout = new MultiItemWidget( GetItem(index), this, viewer, index))
        return layout;
    return NULL;
}

MultiItemWidget::MultiItemWidget(MultiItem *item, MultiItemSource *source, TBSelectItemViewer *source_viewer, int index)
{
    SetSkinBg(TBIDC("TBSelectItem"));
    SetLayoutDistribution(LAYOUT_DISTRIBUTION_GRAVITY);
    SetLayoutDistributionPosition(LAYOUT_DISTRIBUTION_POSITION_LEFT_TOP);
    SetPaintOverflowFadeout(false);

    if (item) // unpack the MultiItem recipe.
    {
        int col = 0;
        int numcols = item->GetNumColumns();
        int prefheight = item->GetColumnHeight();
        for ( col = 0; col < numcols; col++ )
        {
            TBStr widget;
            const char *widgetx = item->GetColumnWidget(col); // what are we making?
            if (widgetx) widget = TBStr(widgetx); // lets not deal with char *s
            if ( widget.Equals("TEXTC") )  // TextField center justified text
            {
                TBTextField *tfc = new TBTextField();
                tfc->SetTextAlign(TB_TEXT_ALIGN::TB_TEXT_ALIGN_CENTER);
                tfc->SetSqueezable(true);
                tfc->SetIgnoreInput(true);
                tfc->SetText( item->GetColumnStr(col));
                LayoutParams *lpx = new LayoutParams();
                lpx->SetWidth(item->GetColumnWidth(col));
                if ( prefheight > 0) lpx->SetHeight( prefheight);
                tfc->SetLayoutParams(*lpx);
                AddChild (tfc);
            }
            else if ( widget.Equals("TEXTR") )  // TextField right justified text
            {
                TBTextField *tfr = new TBTextField();
                tfr->SetTextAlign(TB_TEXT_ALIGN::TB_TEXT_ALIGN_RIGHT);
                tfr->SetSqueezable(true);
                tfr->SetIgnoreInput(true);
                tfr->SetText( item->GetColumnStr(col));
                LayoutParams *lpx = new LayoutParams();
                lpx->SetWidth(item->GetColumnWidth(col));
                if ( prefheight > 0) lpx->SetHeight( prefheight);
                tfr->SetLayoutParams(*lpx);
                AddChild (tfr);
            }
            else if ( widget.Equals("IMAGE") )  // ImageWidget + filename
            {
                TBImageWidget *ti = new TBImageWidget();
                ti->SetImage(item->GetColumnStr(col));
                ti->SetIgnoreInput(true);
                LayoutParams *lpx = new LayoutParams();
                lpx->SetWidth(item->GetColumnWidth(col));
                if ( prefheight > 0) lpx->SetHeight( prefheight);
                ti->SetLayoutParams(*lpx);
                AddChild (ti);
            }
            else if ( widget.Equals("COLOR") )  // ColorWidget
            {
                TBColorWidget *cw = new TBColorWidget();
                cw->SetColor(item->GetColumnStr(col));
                cw->SetIgnoreInput(true);
                cw->SetGravity(WIDGET_GRAVITY_ALL);
                LayoutParams *lpx = new LayoutParams();
                lpx->SetWidth(item->GetColumnWidth(col));
                if ( prefheight > 0) lpx->SetHeight( prefheight);
                cw->SetLayoutParams(*lpx);
                AddChild(cw);
            }
            else if ( widget.Equals("ICON") )   // SkinImage + skinname
            {
                const char *skin = item->GetColumnStr(col);
                if (skin)
                {
                    TBID skinn = TBID(skin);
                    TBSkinImage *si = new TBSkinImage(skinn);
                    si->SetIgnoreInput(true);
                    LayoutParams *lpx = new LayoutParams();
                    lpx->SetWidth(item->GetColumnWidth(col));
                    if (prefheight > 0) lpx->SetHeight(prefheight);
                    si->SetLayoutParams(*lpx);
                    AddChild (si);
                }
            }
            else // the default is TextField, left justified text
            {
                TBTextField *tfl = new TBTextField();
                tfl->SetTextAlign(TB_TEXT_ALIGN::TB_TEXT_ALIGN_LEFT);
                tfl->SetSqueezable(true);
                tfl->SetIgnoreInput(true);
                tfl->SetText( item->GetColumnStr(col));
                LayoutParams *lpx = new LayoutParams();
                lpx->SetWidth(item->GetColumnWidth(col));
                if ( prefheight > 0) lpx->SetHeight(prefheight);
                tfl->SetLayoutParams(*lpx);
                AddChild (tfl);
            }
        }
    }
}

bool MultiItemWidget::OnEvent(const TBWidgetEvent &ev)
{
    // if there are active widgets (buttons, checkbox) handle the events here
    return TBLayout::OnEvent(ev);
}


}; // namespace tb
