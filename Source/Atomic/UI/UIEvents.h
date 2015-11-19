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
EVENT(E_UIUPDATE, UIUpdate)
{

}

EVENT(E_WIDGETEVENT, WidgetEvent)
{
    PARAM(P_HANDLER, Handler);           // UIWidget pointer of widget's OnEvent we are in
    PARAM(P_TARGET, Target);             // UIWidget pointer
    PARAM(P_TYPE, Type);                 // EVENT_TYPE enum
    PARAM(P_X, X);                       // int
    PARAM(P_Y, Y);                       // int
    PARAM(P_DELTAX, DeltaX);             // int
    PARAM(P_DELTAY, DeltaY);             // int
    PARAM(P_COUNT, Count);               // int
    PARAM(P_KEY, Key);                   // int
    PARAM(P_SPECIALKEY, SpecialKey);     // enum SPECIAL_KEY
    PARAM(P_MODIFIERKEYS, ModifierKeys); // enum MODIFIER_KEYS
    PARAM(P_REFID, RefID);               // string (TBID)
    PARAM(P_TOUCH, Touch);               // bool

    // EventHandled can be set by event receivers to stop event bubble
    PARAM(P_HANDLED, Handled);               // [OUT] -> bool

}

EVENT(E_WIDGETLOADED, WidgetLoaded)
{
    PARAM(P_WIDGET, Widget);             // UIWidget pointer
}

EVENT(E_WIDGETFOCUSCHANGED, WidgetFocusChanged)
{
    PARAM(P_WIDGET, Widget);             // UIWidget pointer
    PARAM(P_FOCUSED, Focused);             // bool
}


EVENT(E_WIDGETDELETED, WidgetDeleted)
{
    PARAM(P_WIDGET, Widget);             // UIWidget pointer
}

EVENT(E_DRAGBEGIN, DragBegin)
{
    PARAM(P_TARGET, Source);             // UIWidget source
    PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}

EVENT(E_DRAGENTERWIDGET, DragEnterWidget)
{
    PARAM(P_WIDGET, Widget);             // UIWidget pointer
    PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}

EVENT(E_DRAGEXITWIDGET, DragExitWidget)
{
    PARAM(P_WIDGET, Widget);             // UIWidget pointer
    PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}

EVENT(E_DRAGENDED, DragEnded)
{
    PARAM(P_TARGET, Target);             // UIWidget pointer
    PARAM(P_DRAGOBJECT, DragObject);     // UIDragObject pointer
}


EVENT(E_POPUPMENUSELECT, PopupMenuSelect)
{
    PARAM(P_BUTTON, Button);             // UIButton that created popup
    PARAM(P_REFID, RefID);             // string tbid
}

EVENT(E_UISHORTCUT, UIShortcut)
{
    PARAM(P_KEY, Key);                    // int
    PARAM(P_QUALIFIERS, Qualifiers);        // int

}

EVENT(E_UIWIDGETFOCUSCHANGED, UIWidgetFocusChanged)
{
    PARAM(P_WIDGET, Widget);             // UIWidget pointer
    PARAM(P_FOCUSED, Focused);             // bool
}
EVENT(E_UIWIDGETFOCUSESCAPED, UIWidgetFocusEscaped)
{
}

EVENT(E_UIWIDGETEDITCOMPLETE, UIWidgetEditComplete)
{
    PARAM(P_WIDGET, Widget);             // UIWidget pointer
}

EVENT(E_UIUNHANDLEDSHORTCUT, UIUnhandledShortcut)
{
    PARAM(P_REFID, RefID); // string tbid
}

EVENT(E_UILISTVIEWSELECTIONCHANGED, UIListViewSelectionChanged)
{
    PARAM(P_REFID, RefID); // string tbid
    PARAM(P_SELECTED, Selected);        // bool
}


}
