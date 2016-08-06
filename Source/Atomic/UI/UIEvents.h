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

#pragma once

#include "../Core/Object.h"

namespace Atomic
{

// UIUpdate event
ATOMIC_EVENT(E_UIUPDATE, UIUpdate)
{

}

ATOMIC_EVENT(E_WIDGETEVENT, WidgetEvent)
{
    ATOMIC_PARAM(P_HANDLER, Handler);           // UIWidget pointer of widget's OnEvent we are in
    ATOMIC_PARAM(P_TARGET, Target);             // UIWidget pointer
    ATOMIC_PARAM(P_TYPE, Type);                 // EVENT_TYPE enum
    ATOMIC_PARAM(P_X, X);                       // int
    ATOMIC_PARAM(P_Y, Y);                       // int
    ATOMIC_PARAM(P_DELTAX, DeltaX);             // int
    ATOMIC_PARAM(P_DELTAY, DeltaY);             // int
    ATOMIC_PARAM(P_COUNT, Count);               // int
    ATOMIC_PARAM(P_KEY, Key);                   // int
    ATOMIC_PARAM(P_SPECIALKEY, SpecialKey);     // enum SPECIAL_KEY
    ATOMIC_PARAM(P_MODIFIERKEYS, ModifierKeys); // enum MODIFIER_KEYS
    ATOMIC_PARAM(P_REFID, RefID);               // string (TBID)
    ATOMIC_PARAM(P_TOUCH, Touch);               // bool

    // EventHandled can be set by event receivers to stop event bubble
    ATOMIC_PARAM(P_HANDLED, Handled);               // [OUT] -> bool

}

ATOMIC_EVENT(E_WIDGETLOADED, WidgetLoaded)
{
    ATOMIC_PARAM(P_WIDGET, Widget);             // UIWidget pointer
}

ATOMIC_EVENT(E_WIDGETFOCUSCHANGED, WidgetFocusChanged)
{
    ATOMIC_PARAM(P_WIDGET, Widget);             // UIWidget pointer
    ATOMIC_PARAM(P_FOCUSED, Focused);             // bool
}


ATOMIC_EVENT(E_WIDGETDELETED, WidgetDeleted)
{
    ATOMIC_PARAM(P_WIDGET, Widget);             // UIWidget pointer
}

ATOMIC_EVENT(E_DRAGBEGIN, DragBegin)
{
    ATOMIC_PARAM(P_TARGET, Source);             // UIWidget source
    ATOMIC_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}

ATOMIC_EVENT(E_DRAGENTERWIDGET, DragEnterWidget)
{
    ATOMIC_PARAM(P_WIDGET, Widget);             // UIWidget pointer
    ATOMIC_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}

ATOMIC_EVENT(E_DRAGEXITWIDGET, DragExitWidget)
{
    ATOMIC_PARAM(P_WIDGET, Widget);             // UIWidget pointer
    ATOMIC_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}

ATOMIC_EVENT(E_DRAGENDED, DragEnded)
{
    ATOMIC_PARAM(P_TARGET, Target);             // UIWidget pointer
    ATOMIC_PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}


ATOMIC_EVENT(E_POPUPMENUSELECT, PopupMenuSelect)
{
    ATOMIC_PARAM(P_BUTTON, Button);             // UIButton that created popup
    ATOMIC_PARAM(P_REFID, RefID);             // string tbid
}

ATOMIC_EVENT(E_UISHORTCUT, UIShortcut)
{
    ATOMIC_PARAM(P_KEY, Key);                    // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int

}

ATOMIC_EVENT(E_UIWIDGETFOCUSCHANGED, UIWidgetFocusChanged)
{
    ATOMIC_PARAM(P_WIDGET, Widget);             // UIWidget pointer
    ATOMIC_PARAM(P_FOCUSED, Focused);             // bool
}
ATOMIC_EVENT(E_UIWIDGETFOCUSESCAPED, UIWidgetFocusEscaped)
{
}

ATOMIC_EVENT(E_UIWIDGETEDITCOMPLETE, UIWidgetEditComplete)
{
    ATOMIC_PARAM(P_WIDGET, Widget);             // UIWidget pointer
}

ATOMIC_EVENT(E_UIUNHANDLEDSHORTCUT, UIUnhandledShortcut)
{
    ATOMIC_PARAM(P_REFID, RefID); // string tbid
}

ATOMIC_EVENT(E_UILISTVIEWSELECTIONCHANGED, UIListViewSelectionChanged)
{
    ATOMIC_PARAM(P_REFID, RefID); // string tbid
    ATOMIC_PARAM(P_SELECTED, Selected);        // bool
}


}
