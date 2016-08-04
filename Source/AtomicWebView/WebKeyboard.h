//
// Copyright (c) 2014-2016, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Core/Variant.h>

namespace Atomic
{

class Input;

/// Convert an Atomic KeyEvent to a CefKeyEvent, returns true if processing should continue
bool ConvertKeyEvent(Input* input, const StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent);
/// Convert an Atomic TextInput to a CefKeyEvent, returns true if processing should continue
bool ConvertTextInputEvent(const StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent);


/// A convenience structure to "parse" a KeyUp/KeyDown event
struct WebKeyEvent
{
    bool repeat;
    int key;
    int scanCode;
    int buttons;
    int qual;
    bool keyDown;

    WebKeyEvent(const StringHash eventType, VariantMap& eventData)
    {
        if (eventType == "KeyDown")
        {
            keyDown = true;
            repeat = eventData[KeyDown::P_REPEAT].GetBool();
            key = eventData[KeyDown::P_KEY].GetInt();
            scanCode = eventData[KeyDown::P_SCANCODE].GetInt();
            buttons = eventData[KeyDown::P_BUTTONS].GetInt();
            qual = eventData[KeyDown::P_QUALIFIERS].GetInt();
        }
        else if (eventType == "KeyUp")
        {
            keyDown = false;
            key = eventData[KeyUp::P_KEY].GetInt();
            scanCode = eventData[KeyUp::P_SCANCODE].GetInt();
            buttons = eventData[KeyUp::P_BUTTONS].GetInt();
            qual = eventData[KeyUp::P_QUALIFIERS].GetInt();
        }

    }
};

}
