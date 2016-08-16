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

#include "../IO/Log.h"
#include "../Input/InputEvents.h"

#include "UIEvents.h"
#include "UI.h"
#include "UIWidget.h"
#include "UILayout.h"
#include "UIFontDescription.h"
#include "UIView.h"

using namespace tb;

namespace Atomic
{

UIWidget::UIWidget(Context* context, bool createWidget) : Object(context),
    widget_(0),
    preferredSize_(new UIPreferredSize()),
    multiTouch_(false)
{
    if (createWidget)
    {
        widget_ = new TBWidget();
        widget_->SetDelegate(this);
        GetSubsystem<UI>()->WrapWidget(this, widget_);
    }    

}

UIWidget::~UIWidget()
{

}

void UIWidget::SetIsFocusable(bool value)
{
    if (!widget_)
        return;

    widget_->SetIsFocusable(value);

}

bool UIWidget::Load(const String& filename)
{
    UI* ui = GetSubsystem<UI>();

    if  (!ui->LoadResourceFile(widget_ , filename))
        return false;

    VariantMap eventData;
    eventData[WidgetLoaded::P_WIDGET] = this;
    SendEvent(E_WIDGETLOADED, eventData);
    return true;
}

UIPreferredSize* UIWidget::GetPreferredSize()
{
    // error
    if (!widget_)
        return preferredSize_;

    preferredSize_->SetFromTBPreferredSize(widget_->GetPreferredSize());

    return preferredSize_;
}

UIWidget* UIWidget::GetWidget(const String& id)
{
    if (!widget_)
        return 0;

    TBWidget* child = widget_->GetWidgetByID(TBID(id.CString()));

    if (!child)
        return 0;

    UI* ui = GetSubsystem<UI>();
    return ui->WrapWidget(child);
}

void UIWidget::SetWidget(tb::TBWidget* widget)
{
    widget_ = widget;
    widget_->SetDelegate(this);
}

/*
enum SPECIAL_KEY
{
    TB_KEY_UNDEFINED = 0,
    TB_KEY_UP, TB_KEY_DOWN, TB_KEY_LEFT, TB_KEY_RIGHT,
    TB_KEY_PAGE_UP, TB_KEY_PAGE_DOWN, TB_KEY_HOME, TB_KEY_END,
    TB_KEY_TAB, TB_KEY_BACKSPACE, TB_KEY_INSERT, TB_KEY_DELETE,
    TB_KEY_ENTER, TB_KEY_ESC,
    TB_KEY_F1, TB_KEY_F2, TB_KEY_F3, TB_KEY_F4, TB_KEY_F5, TB_KEY_F6,
    TB_KEY_F7, TB_KEY_F8, TB_KEY_F9, TB_KEY_F10, TB_KEY_F11, TB_KEY_F12
};
*/


void UIWidget::ConvertEvent(UIWidget *handler, UIWidget* target, const tb::TBWidgetEvent &ev, VariantMap& data)
{
    UI* ui = GetSubsystem<UI>();

    String refid;

    ui->GetTBIDString(ev.ref_id, refid);

    int key = ev.key;

    if (ev.special_key)
    {
        switch (ev.special_key)
        {
        case TB_KEY_ENTER:
            key = KEY_RETURN;
            break;
        case TB_KEY_BACKSPACE:
            key = KEY_BACKSPACE;
            break;
        case TB_KEY_DELETE:
            key = KEY_DELETE;
            break;
        case TB_KEY_DOWN:
            key = KEY_DOWN;
            break;
        case TB_KEY_UP:
            key = KEY_UP;
            break;
        case TB_KEY_LEFT:
            key = KEY_LEFT;
            break;
        case TB_KEY_RIGHT:
            key = KEY_RIGHT;
            break;
        default:
            break;
        }
    }

    unsigned modifierKeys = 0;

    if( ev.special_key && TB_SHIFT)
        modifierKeys |= QUAL_SHIFT;
    if( ev.special_key && TB_CTRL)
        modifierKeys |= QUAL_CTRL;
    if( ev.special_key && TB_ALT)
        modifierKeys |= QUAL_ALT;

    using namespace WidgetEvent;
    data[P_HANDLER] = handler;
    data[P_TARGET] = target;
    data[P_TYPE] = (unsigned) ev.type;
    data[P_X] = ev.target_x;
    data[P_Y] = ev.target_y;
    data[P_DELTAX] = ev.delta_x;
    data[P_DELTAY] = ev.delta_y;
    data[P_COUNT] = ev.count;
    data[P_KEY] = key;

    data[P_SPECIALKEY] = (unsigned) ev.special_key;
    data[P_MODIFIERKEYS] = modifierKeys;
    data[P_REFID] = refid;
    data[P_TOUCH] = (unsigned) ev.touch;
}

void UIWidget::OnDelete()
{
    VariantMap eventData;
    eventData[WidgetDeleted::P_WIDGET] = this;
    SendEvent(E_WIDGETDELETED, eventData);

    UnsubscribeFromAllEvents();   

    if (widget_)
    {
        // if we don't have a UI subsystem, we are exiting
        UI* ui = GetSubsystem<UI>();

        if (ui)
        { 
            if (ui->UnwrapWidget(widget_))
            {
                widget_ = 0;
                ReleaseRef();
                return;
            }
        }
    }

    widget_ = 0;

}

void UIWidget::AddChildAfter(UIWidget* child, UIWidget* otherChild)
{
    if (!widget_ || !child || !child->widget_ || !otherChild || !otherChild->widget_)
        return;

    widget_->AddChildRelative(child->widget_, tb::WIDGET_Z_REL_AFTER, otherChild->widget_);

}

void UIWidget::AddChildBefore(UIWidget* child, UIWidget* otherChild)
{
    if (!widget_ || !child || !child->widget_ || !otherChild || !otherChild->widget_)
        return;

    widget_->AddChildRelative(child->widget_, tb::WIDGET_Z_REL_BEFORE, otherChild->widget_);

}

void UIWidget::AddChild(UIWidget* child)
{
    if (!widget_ || !child || !child->widget_)
        return;

    widget_->AddChild(child->widget_);
}

void UIWidget::AddChildRelative(UIWidget* child, UI_WIDGET_Z_REL z, UIWidget* reference)
{
    if (!widget_ || !child || !child->widget_ || !reference || !reference->widget_)
        return;

    widget_->AddChildRelative(child->widget_, (WIDGET_Z_REL) z, reference->widget_);

}

String UIWidget::GetText()
{
    if (!widget_)
        return String::EMPTY;

    return widget_->GetText().CStr();
}

void UIWidget::SetText(const String& text)
{
    if (!widget_)
        return;

    widget_->SetText(text.CString());
}

void UIWidget::SetGravity(UI_GRAVITY gravity)
{
    if (!widget_)
        return;

    widget_->SetGravity((WIDGET_GRAVITY) gravity);

}

void UIWidget::SetAxis(UI_AXIS axis)
{
    if (!widget_)
        return;

    widget_->SetAxis((AXIS) axis);
}

bool UIWidget::IsAncestorOf(UIWidget* widget)
{
    if (!widget_ || !widget || !widget->widget_)
        return false;

    return widget_->IsAncestorOf(widget->widget_);

}

void UIWidget::SetPosition(int x, int y)
{
    if (!widget_)
        return;

    widget_->SetPosition(TBPoint(x, y));

}

IntRect UIWidget::GetRect()
{
    IntRect rect(0, 0, 0, 0);

    if (!widget_)
        return rect;

    tb::TBRect tbrect = widget_->GetRect();

    rect.top_ = tbrect.y;
    rect.left_ = tbrect.x;
    rect.right_ = tbrect.x + tbrect.w;
    rect.bottom_ = tbrect.y + tbrect.h;

    return rect;
}

void UIWidget::SetRect(IntRect rect)
{
    if (!widget_)
        return;

    tb::TBRect tbrect;

    tbrect.y = rect.top_;
    tbrect.x = rect.left_;
    tbrect.w = rect.right_ - rect.left_;
    tbrect.h = rect.bottom_ - rect.top_;

    widget_->SetRect(tbrect);

}


void UIWidget::SetSize(int width, int height)
{
    if (!widget_)
        return;

    widget_->SetSize(width, height);
}

void UIWidget::Invalidate()
{
    if (!widget_)
        return;

    widget_->Invalidate();
}

void UIWidget::Center()
{
    if (!widget_)
        return;

    TBRect rect = widget_->GetRect();
    TBWidget* root = widget_->GetParent();
    if (!root)
    {
        UI* ui = GetSubsystem<UI>();
        root = ui->GetRootWidget();
    }

    TBRect bounds(0, 0, root->GetRect().w, root->GetRect().h);
    widget_->SetRect(rect.CenterIn(bounds).MoveIn(bounds).Clip(bounds));

}

UIWidget* UIWidget::GetParent()
{
    if (!widget_)
        return 0;

    TBWidget* parent = widget_->GetParent();

    if (!parent)
        return 0;

    UI* ui = GetSubsystem<UI>();

    return ui->WrapWidget(parent);

}

UIWidget* UIWidget::GetContentRoot()
{
    if (!widget_)
        return 0;

    TBWidget* root = widget_->GetContentRoot();

    if (!root)
        return 0;

    UI* ui = GetSubsystem<UI>();

    return ui->WrapWidget(root);
}

void UIWidget::Die()
{
    if (!widget_)
        return;

    // clear delegate
    widget_->SetDelegate(NULL);
    // explictly die (can trigger an animation)
    widget_->Die();
    // call OnDelete, which unwraps the widget and does some bookkeeping
    OnDelete();

}

void UIWidget::SetLayoutParams(UILayoutParams* params)
{
    if (!widget_)
        return;

    widget_->SetLayoutParams(*(params->GetTBLayoutParams()));

}

void UIWidget::SetFontDescription(UIFontDescription* fd)
{
    if (!widget_)
        return;

    widget_->SetFontDescription(*(fd->GetTBFontDescription()));

}

void UIWidget::SetFontId(const String& fontId)
{
    if (!widget_)
        return;

    tb::TBFontDescription fd(widget_->GetFontDescription());
    fd.SetID(fontId.CString());
    widget_->SetFontDescription(fd);
}

String UIWidget::GetFontId()
{
    if (!widget_)
        return "";

    tb::TBFontDescription fd(widget_->GetFontDescription());
    if (!g_font_manager->HasFontFace(fd))
    {
        return "";
    }
    return g_font_manager->GetFontInfo(fd.GetID())->GetName();
}

void UIWidget::SetFontSize(int size)
{
    if (!widget_)
        return;

    tb::TBFontDescription fd(widget_->GetFontDescription());
    fd.SetSize(size);
    widget_->SetFontDescription(fd);
}

int UIWidget::GetFontSize()
{
    if (!widget_)
        return 0;

    tb::TBFontDescription fd(widget_->GetFontDescription());
    return fd.GetSize();
}

void UIWidget::SetLayoutWidth(int width)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.SetWidth(width);
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutWidth()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->pref_w;
}

void UIWidget::SetLayoutHeight(int height)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.SetHeight(height);
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutHeight()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->pref_h;
}

void UIWidget::SetLayoutPrefWidth(int width)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.pref_w = width;
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutPrefWidth()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->pref_w;
}

void UIWidget::SetLayoutPrefHeight(int height)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.pref_h = height;
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutPrefHeight()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->pref_h;
}

void UIWidget::SetLayoutMinWidth(int width)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.min_w = width;
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutMinWidth()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->min_w;
}

void UIWidget::SetLayoutMinHeight(int height)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.min_h = height;
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutMinHeight()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->min_h;
}

void UIWidget::SetLayoutMaxWidth(int width)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.max_w = width;
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutMaxWidth()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->max_w;
}

void UIWidget::SetLayoutMaxHeight(int height)
{
    if (!widget_)
        return;

    tb::LayoutParams lp;

    const tb::LayoutParams *oldLp(widget_->GetLayoutParams());
    if (oldLp)
        lp = *oldLp;

    lp.max_h = height;
    widget_->SetLayoutParams(lp);
}

int UIWidget::GetLayoutMaxHeight()
{
    if (!widget_)
        return tb::LayoutParams::UNSPECIFIED;

    const tb::LayoutParams *lp(widget_->GetLayoutParams());
    if (!lp)
        return tb::LayoutParams::UNSPECIFIED;

    return lp->max_h;
}

void UIWidget::SetOpacity(float opacity)
{
    if (!widget_)
        return;

    widget_->SetOpacity(opacity);
}

float UIWidget::GetOpacity()
{
    if (!widget_)
        return -0.0f;

    return widget_->GetOpacity();
}

void UIWidget::SetAutoOpacity(float autoOpacity)
{
    if (!widget_)
        return;

    if (autoOpacity == 0.0f)
    {
        widget_->SetOpacity(autoOpacity);
        widget_->SetVisibilility(tb::WIDGET_VISIBILITY_INVISIBLE);
    }
    else
    {
        widget_->SetVisibilility(tb::WIDGET_VISIBILITY_VISIBLE);
        widget_->SetOpacity(autoOpacity);
    }
}

float UIWidget::GetAutoOpacity()
{
    if (!widget_)
        return -0.0f;

    float autoOpacity(widget_->GetOpacity());

    if (autoOpacity == 0.0f)
    {
        if (widget_->GetVisibility() == tb::WIDGET_VISIBILITY_VISIBLE)
            return 0.0001f; // Don't say that it's completly invisible.
    }
    else
    {
        if (widget_->GetVisibility() != tb::WIDGET_VISIBILITY_VISIBLE)
            return 0.0f; // Say it's invisible.
    }
    return autoOpacity;
}

void UIWidget::DeleteAllChildren()
{
    if (!widget_)
        return;

    widget_->DeleteAllChildren();
}

void UIWidget::SetSkinBg(const String& id)
{
    if (!widget_)
        return;

    widget_->SetSkinBg(TBIDC(id.CString()));

}

void UIWidget::Remove()
{
    if (!widget_ || !widget_->GetParent())
        return;

    widget_->GetParent()->RemoveChild(widget_);

}

void UIWidget::RemoveChild(UIWidget* child, bool cleanup)
{
    if (!widget_ || !child)
        return;

    TBWidget* childw = child->GetInternalWidget();

    if (!childw)
        return;

    widget_->RemoveChild(childw);

    if (cleanup)
        delete childw;
}


const String& UIWidget::GetId()
{
    if (!widget_ || !widget_->GetID())
    {
        if (id_.Length())
            id_.Clear();

        return id_;
    }

    if (id_.Length())
        return id_;

    UI* ui = GetSubsystem<UI>();
    ui->GetTBIDString(widget_->GetID(), id_);

    return id_;

}

void UIWidget::SetId(const String& id)
{
    if (!widget_)
    {
        if (id_.Length())
            id_.Clear();

        return;
    }

    id_ = id;
    widget_->SetID(TBIDC(id.CString()));

}

void UIWidget::SetState(UI_WIDGET_STATE state, bool on)
{
    if (!widget_)
        return;

    widget_->SetState((WIDGET_STATE) state, on);
}

void UIWidget::SetFocus()
{
    if (!widget_)
        return;

    widget_->SetFocus(WIDGET_FOCUS_REASON_UNKNOWN);

}

bool UIWidget::GetFocus()
{
    if (!widget_)
        return false;

    return widget_->GetIsFocused();
}

void UIWidget::SetFocusRecursive()
{
    if (!widget_)
        return;

    widget_->SetFocusRecursive(WIDGET_FOCUS_REASON_UNKNOWN);
}

void UIWidget::SetVisibility(UI_WIDGET_VISIBILITY visibility)
{

    if (!widget_)
        return;

    widget_->SetVisibilility((WIDGET_VISIBILITY) visibility);

}

UI_WIDGET_VISIBILITY UIWidget::GetVisibility()
{
    if (!widget_)
        return UI_WIDGET_VISIBILITY_GONE;

    return (UI_WIDGET_VISIBILITY) widget_->GetVisibility();
}

UIWidget* UIWidget::GetFirstChild()
{
    if (!widget_)
        return NULL;

    return GetSubsystem<UI>()->WrapWidget(widget_->GetFirstChild());

}

UIWidget* UIWidget::GetNext()
{
    if (!widget_)
        return NULL;

    return GetSubsystem<UI>()->WrapWidget(widget_->GetNext());

}

void UIWidget::SetValue(double value)
{
    if (!widget_)
        return;

    widget_->SetValueDouble(value);
}

double UIWidget::GetValue()
{
    if (!widget_)
        return 0.0;

    return widget_->GetValueDouble();

}

void UIWidget::Enable()
{
    if (!widget_)
        return;

    widget_->SetState(WIDGET_STATE_DISABLED, false);

}

void UIWidget::Disable()
{

    if (!widget_)
        return;

    widget_->SetState(WIDGET_STATE_DISABLED, true);

}

bool UIWidget::GetState(UI_WIDGET_STATE state)
{
    if (!widget_)
        return false;

    return widget_->GetState((WIDGET_STATE) state);

}

void UIWidget::SetStateRaw(UI_WIDGET_STATE state)
{
    if (!widget_)
        return;

    widget_->SetStateRaw((WIDGET_STATE) state);

}

UI_WIDGET_STATE UIWidget::GetStateRaw()
{
    if (!widget_)
        return UI_WIDGET_STATE_NONE;

    return (UI_WIDGET_STATE) widget_->GetStateRaw();

}

UIView* UIWidget::GetView()
{
    if (!widget_)
        return 0;

    if (GetType() == UIView::GetTypeStatic())
        return (UIView*) this;

    TBWidget* tbw = widget_->GetParent();
    while(tbw)
    {
        TBWidgetDelegate* delegate = tbw->GetDelegate();
        if (delegate)
        {
            UIWidget* d = (UIWidget*) delegate;
            if (d->GetType() == UIView::GetTypeStatic())
                return (UIView*) d;
        }

        tbw = tbw->GetParent();
    }

    return 0;
}

void UIWidget::OnFocusChanged(bool focused)
{
    using namespace WidgetFocusChanged;

    VariantMap eventData;
    eventData[P_WIDGET] = this;
    eventData[P_FOCUSED] = focused;
    SendEvent(E_WIDGETFOCUSCHANGED, eventData);

}

bool UIWidget::OnEvent(const tb::TBWidgetEvent &ev)
{
    UI* ui = GetSubsystem<UI>();

    if ((ev.type == EVENT_TYPE_CHANGED && !ui->GetBlockChangedEvents()) || ev.type == EVENT_TYPE_KEY_UP)
    {
        if (!ev.target || ui->IsWidgetWrapped(ev.target))
        {
            VariantMap eventData;
            ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
            SendEvent(E_WIDGETEVENT, eventData);

            if (eventData[WidgetEvent::P_HANDLED].GetBool())
                return true;

        }

    }
    else if (ev.type == EVENT_TYPE_RIGHT_POINTER_UP)
    {
        if (!ev.target || ui->IsWidgetWrapped(ev.target))
        {
            VariantMap eventData;
            ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
            SendEvent(E_WIDGETEVENT, eventData);

            if (eventData[WidgetEvent::P_HANDLED].GetBool())
                return true;

        }

    }
    else if (ev.type == EVENT_TYPE_POINTER_DOWN)
    {
        if (!ev.target || ui->IsWidgetWrapped(ev.target))
        {
            VariantMap eventData;
            ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
            SendEvent(E_WIDGETEVENT, eventData);

            if (eventData[WidgetEvent::P_HANDLED].GetBool())
                return true;

        }

    }
    else if (ev.type == EVENT_TYPE_SHORTCUT)
    {
        if (!ev.target || ui->IsWidgetWrapped(ev.target))
        {
            VariantMap eventData;
            ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
            SendEvent(E_WIDGETEVENT, eventData);

            if (eventData[WidgetEvent::P_HANDLED].GetBool())
                return true;

        }

    }
    else if (ev.type == EVENT_TYPE_TAB_CHANGED)
    {
        if (!ev.target || ui->IsWidgetWrapped(ev.target))
        {
            VariantMap eventData;
            ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
            SendEvent(E_WIDGETEVENT, eventData);

            if (eventData[WidgetEvent::P_HANDLED].GetBool())
                return true;

        }

    }
    else if (ev.type == EVENT_TYPE_CLICK)
    {
        if (ev.target && ev.target->GetID() == TBID("__popup-menu"))
        {
            // popup menu

            if (JSGetHeapPtr())
            {
                VariantMap eventData;
                eventData[PopupMenuSelect::P_BUTTON] = this;
                String id;
                ui->GetTBIDString(ev.ref_id, id);
                eventData[PopupMenuSelect::P_REFID] = id;
                SendEvent(E_POPUPMENUSELECT, eventData);
            }

            return true;
        }
        else
        {
            if (!ev.target || ui->IsWidgetWrapped(ev.target))
            {
                VariantMap eventData;
                ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
                SendEvent(E_WIDGETEVENT, eventData);

                if (eventData[WidgetEvent::P_HANDLED].GetBool())
                    return true;

            }
        }

    }
    if (ev.type == EVENT_TYPE_CUSTOM)
    {
        if (!ev.target || ui->IsWidgetWrapped(ev.target))
        {
            VariantMap eventData;
            ConvertEvent(this, ui->WrapWidget(ev.target), ev, eventData);
            SendEvent(E_WIDGETEVENT, eventData);

            if (eventData[WidgetEvent::P_HANDLED].GetBool())
                return true;

        }

    }


    return false;
}

bool UIWidget::GetCaptured()
{
    if (!widget_)
        return false;

    return widget_->IsCaptured();

}

void UIWidget::SetCapturing(bool capturing)
{
    if (!widget_)
        return;

    widget_->SetCapturing(capturing);
}

bool UIWidget::GetCapturing()
{
    if (!widget_)
        return false;

    return widget_->GetCapturing();
}

void UIWidget::InvalidateLayout()
{
    if (!widget_)
        return;

    widget_->InvalidateLayout(tb::TBWidget::INVALIDATE_LAYOUT_TARGET_ONLY);

}

void UIWidget::InvokeShortcut(const String& shortcut)
{
    TBWidgetEvent ev(EVENT_TYPE_SHORTCUT);
    ev.ref_id = TBIDC(shortcut.CString());
    widget_->OnEvent(ev);
}

bool UIWidget::GetShortened()
{
    if (!widget_)
        return false;

    return widget_->GetShortened();
}

void UIWidget::SetShortened(bool shortened)
{
    if (!widget_)
        return;

    widget_->SetShortened(shortened);
}

String UIWidget::GetTooltip()
{
    if (!widget_)
        return String::EMPTY;

    return widget_->GetTooltip().CStr();
}

void UIWidget::SetTooltip(const String& tooltip)
{
    if (!widget_)
        return;

    widget_->SetTooltip(tooltip.CString());
}


}
