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

#ifdef ATOMIC_PLATFORM_LINUX

#include <include/cef_client.h>

#include <Atomic/Core/Variant.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Input/Input.h>
#include <Atomic/IO/Log.h>

#include "WebKeyboard.h"

namespace Atomic
{

bool ConvertKeyEvent(Input* input, const StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent)
{
    memset((void*)&keyEvent, 0, sizeof(keyEvent));
    
    WebKeyEvent wk(eventType, eventData); 

    keyEvent.type = wk.keyDown ? KEYEVENT_KEYDOWN : KEYEVENT_KEYUP;
    keyEvent.is_system_key = false;
    keyEvent.windows_key_code = 0;
    keyEvent.focus_on_editable_field = false;
    keyEvent.modifiers = EVENTFLAG_NONE;
    if (wk.qual & QUAL_SHIFT) keyEvent.modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (wk.qual & QUAL_ALT) keyEvent.modifiers |= EVENTFLAG_ALT_DOWN;
    if (wk.qual & QUAL_CTRL) keyEvent.modifiers |= EVENTFLAG_CONTROL_DOWN;
    
    keyEvent.native_key_code = wk.scanCode;
    keyEvent.character = wk.key;
    keyEvent.unmodified_character = wk.raw;

    return true;
}

bool ConvertTextInputEvent(StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent)
{
   if (eventType != "TextInput")
    {
        LOGERROR("ConvertTextInputEvent - Unknown event type");
        return false;
    }

    memset((void*)&keyEvent, 0, sizeof(keyEvent));

    String text = eventData[TextInput::P_TEXT].GetString();
 
    if ( text.Length() > 0)
    {
		keyEvent.character = (char16)text[0];
		keyEvent.type = KEYEVENT_CHAR;
		return true; 
	}
    return false;
}

}

#endif
