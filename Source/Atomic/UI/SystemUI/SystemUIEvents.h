//
// Copyright (c) 2008-2015 the Urho3D project.
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

#include "../../Core/Object.h"

namespace Atomic
{

namespace SystemUI
{

/// Mouse click in the UI.
ATOMIC_EVENT(E_UIMOUSECLICK, UIMouseClick)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_BUTTON, Button);                // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Mouse click end in the UI.
ATOMIC_EVENT(E_UIMOUSECLICKEND, UIMouseClickEnd)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_BEGINELEMENT, BeginElement);    // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_BUTTON, Button);                // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Mouse double click in the UI.
ATOMIC_EVENT(E_UIMOUSEDOUBLECLICK, UIMouseDoubleClick)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_BUTTON, Button);                // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Drag and drop test.
ATOMIC_EVENT(E_DRAGDROPTEST, DragDropTest)
{
    ATOMIC_PARAM(P_SOURCE, Source);                // UIElement pointer
    ATOMIC_PARAM(P_TARGET, Target);                // UIElement pointer
    ATOMIC_PARAM(P_ACCEPT, Accept);                // bool
};

/// Drag and drop finish.
ATOMIC_EVENT(E_DRAGDROPFINISH, DragDropFinish)
{
    ATOMIC_PARAM(P_SOURCE, Source);                // UIElement pointer
    ATOMIC_PARAM(P_TARGET, Target);                // UIElement pointer
    ATOMIC_PARAM(P_ACCEPT, Accept);                // bool
};

/// Focus element changed.
ATOMIC_EVENT(E_FOCUSCHANGED, FocusChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_CLICKEDELEMENT, ClickedElement); // UIElement pointer
}

/// UI element name changed.
ATOMIC_EVENT(E_NAMECHANGED, NameChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// UI element resized.
ATOMIC_EVENT(E_RESIZED, Resized)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_WIDTH, Width);                  // int
    ATOMIC_PARAM(P_HEIGHT, Height);                // int
}

/// UI element positioned.
ATOMIC_EVENT(E_POSITIONED, Positioned)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
}

/// UI element visibility changed.
ATOMIC_EVENT(E_VISIBLECHANGED, VisibleChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_VISIBLE, Visible);              // bool
}

/// UI element focused.
ATOMIC_EVENT(E_FOCUSED, Focused)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_BYKEY, ByKey);                  // bool
}

/// UI element defocused.
ATOMIC_EVENT(E_DEFOCUSED, Defocused)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// UI element layout updated.
ATOMIC_EVENT(E_LAYOUTUPDATED, LayoutUpdated)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// UI button pressed.
ATOMIC_EVENT(E_PRESSED, Pressed)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// UI button was pressed, then released.
ATOMIC_EVENT(E_RELEASED, Released)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// UI checkbox toggled.
ATOMIC_EVENT(E_TOGGLED, Toggled)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_STATE, State);                  // bool
}

/// UI slider value changed
ATOMIC_EVENT(E_SLIDERCHANGED, SliderChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_VALUE, Value);                  // float
}

/// UI slider being paged.
ATOMIC_EVENT(E_SLIDERPAGED, SliderPaged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_OFFSET, Offset);                // int
    ATOMIC_PARAM(P_PRESSED, Pressed);              // bool
}

/// UI scrollbar value changed.
ATOMIC_EVENT(E_SCROLLBARCHANGED, ScrollBarChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_VALUE, Value);                  // float
}

/// UI scrollview position changed.
ATOMIC_EVENT(E_VIEWCHANGED, ViewChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
}

/// UI modal changed (currently only Window has modal flag).
ATOMIC_EVENT(E_MODALCHANGED, ModalChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_MODAL, Modal);                  // bool
}

/// Text entry into a LineEdit. The char can be modified in the event data.
ATOMIC_EVENT(E_TEXTENTRY, CharEntry)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_TEXT, Text);                    // String
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Editable text changed
ATOMIC_EVENT(E_TEXTCHANGED, TextChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_TEXT, Text);                    // String
}

/// Text editing finished (enter pressed on a LineEdit)
ATOMIC_EVENT(E_TEXTFINISHED, TextFinished)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_TEXT, Text);                    // String
    ATOMIC_PARAM(P_VALUE, Value);                 // Float
}

/// Menu selected.
ATOMIC_EVENT(E_MENUSELECTED, MenuSelected)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// Listview or DropDownList item selected.
ATOMIC_EVENT(E_ITEMSELECTED, ItemSelected)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_SELECTION, Selection);          // int
}

/// Listview item deselected.
ATOMIC_EVENT(E_ITEMDESELECTED, ItemDeselected)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_SELECTION, Selection);          // int
}

/// Listview selection change finished.
ATOMIC_EVENT(E_SELECTIONCHANGED, SelectionChanged)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// Listview item clicked. If this is a left-click, also ItemSelected event will be sent. If this is a right-click, only this event is sent.
ATOMIC_EVENT(E_ITEMCLICKED, ItemClicked)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_ITEM, Item);                    // UIElement pointer
    ATOMIC_PARAM(P_SELECTION, Selection);          // int
    ATOMIC_PARAM(P_BUTTON, Button);                // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Listview item double clicked.
ATOMIC_EVENT(E_ITEMDOUBLECLICKED, ItemDoubleClicked)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_ITEM, Item);                    // UIElement pointer
    ATOMIC_PARAM(P_SELECTION, Selection);          // int
    ATOMIC_PARAM(P_BUTTON, Button);                // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// LineEdit or ListView unhandled key pressed.
ATOMIC_EVENT(E_UNHANDLEDKEY, UnhandledKey)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_KEY, Key);                      // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_QUALIFIERS, Qualifiers);        // int
}

/// Fileselector choice.
ATOMIC_EVENT(E_FILESELECTED, FileSelected)
{
    ATOMIC_PARAM(P_FILENAME, FileName);            // String
    ATOMIC_PARAM(P_FILTER, Filter);                // String
    ATOMIC_PARAM(P_OK, Ok);                        // bool
}

/// MessageBox acknowlegement.
ATOMIC_EVENT(E_MESSAGEACK, MessageACK)
{
    ATOMIC_PARAM(P_OK, Ok);                        // bool
}

/// A child element has been added to an element. Sent by the UI root element, or element-event-sender if set.
ATOMIC_EVENT(E_ELEMENTADDED, ElementAdded)
{
    ATOMIC_PARAM(P_ROOT, Root);                    // UIElement pointer
    ATOMIC_PARAM(P_PARENT, Parent);                // UIElement pointer
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// A child element is about to be removed from an element. Sent by the UI root element, or element-event-sender if set.
ATOMIC_EVENT(E_ELEMENTREMOVED, ElementRemoved)
{
    ATOMIC_PARAM(P_ROOT, Root);                    // UIElement pointer
    ATOMIC_PARAM(P_PARENT, Parent);                // UIElement pointer
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// Hovering on an UI element has started
ATOMIC_EVENT(E_HOVERBEGIN, HoverBegin)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_ELEMENTX, ElementX);            // int
    ATOMIC_PARAM(P_ELEMENTY, ElementY);            // int
}

/// Hovering on an UI element has ended
ATOMIC_EVENT(E_HOVEREND, HoverEnd)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
}

/// Drag behavior of a UI Element has started
ATOMIC_EVENT(E_DRAGBEGIN, DragBegin)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_ELEMENTX, ElementX);            // int
    ATOMIC_PARAM(P_ELEMENTY, ElementY);            // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_NUMBUTTONS, NumButtons);        // int
}

/// Drag behavior of a UI Element when the input device has moved
ATOMIC_EVENT(E_DRAGMOVE, DragMove)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_DX, DX);                        // int
    ATOMIC_PARAM(P_DY, DY);                        // int
    ATOMIC_PARAM(P_ELEMENTX, ElementX);            // int
    ATOMIC_PARAM(P_ELEMENTY, ElementY);            // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_NUMBUTTONS, NumButtons);        // int
}

/// Drag behavior of a UI Element has finished
ATOMIC_EVENT(E_DRAGEND, DragEnd)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_ELEMENTX, ElementX);            // int
    ATOMIC_PARAM(P_ELEMENTY, ElementY);            // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_NUMBUTTONS, NumButtons);        // int
}

/// Drag of a UI Element was canceled by pressing ESC
ATOMIC_EVENT(E_DRAGCANCEL, DragCancel)
{
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_ELEMENTX, ElementX);            // int
    ATOMIC_PARAM(P_ELEMENTY, ElementY);            // int
    ATOMIC_PARAM(P_BUTTONS, Buttons);              // int
    ATOMIC_PARAM(P_NUMBUTTONS, NumButtons);        // int
}

/// A file was drag-dropped into the application window. Includes also coordinates and UI element if applicable
ATOMIC_EVENT(E_UIDROPFILE, UIDropFile)
{
    ATOMIC_PARAM(P_FILENAME, FileName);            // String
    ATOMIC_PARAM(P_ELEMENT, Element);              // UIElement pointer
    ATOMIC_PARAM(P_X, X);                          // int
    ATOMIC_PARAM(P_Y, Y);                          // int
    ATOMIC_PARAM(P_ELEMENTX, ElementX);            // int (only if element is non-null)
    ATOMIC_PARAM(P_ELEMENTY, ElementY);            // int (only if element is non-null)
}

ATOMIC_EVENT(E_CONSOLECLOSED, ConsoleClosed)
{

}

}

}
