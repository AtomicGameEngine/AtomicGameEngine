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

#ifdef ATOMIC_PLATFORM_OSX

#include <include/cef_client.h>

#include <ThirdParty/SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>
#include <ThirdParty/SDL/src/events/scancodes_darwin.h>

#include <Atomic/Core/Variant.h>
#include <Atomic/Input/InputEvents.h>
#include <Atomic/Input/Input.h>
#include <Atomic/IO/Log.h>

#include "WebKeyboard.h"

namespace Atomic
{

static bool SDLScanCodeToDarwinScanCode(SDL_Scancode code, int& darwinScanCode)
{

    darwinScanCode = -1;

    //if (code >= SDL_SCANCODE_A && code <= SDL_SCANCODE_0)
    //    return false;

    int numCodes = sizeof(darwin_scancode_table)/sizeof(SDL_Scancode);

    for (int i  = 0; i < numCodes; i++)
    {
        if (darwin_scancode_table[i] == code)
        {
            darwinScanCode = i;
            break;
        }
    }

    if (darwinScanCode != -1)
        return true;

    return false;

}

bool ConvertKeyEvent(Input* input, const StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent)
{
    if (eventType != "KeyDown" && eventType != "KeyUp")
    {
        ATOMIC_LOGERROR("ConvertKeyEvent - Unknown event type");
        return false;
    }

    if (eventType == "KeyUp")
        return false;

    memset((void*)&keyEvent, 0, sizeof(keyEvent));

    WebKeyEvent wk(eventType, eventData);

    if (wk.scanCode == SDL_SCANCODE_RETURN)
    {
        if (!wk.keyDown)
            return false;

        keyEvent.native_key_code = 36;
        keyEvent.type = KEYEVENT_RAWKEYDOWN;
        return true;
    }

    keyEvent.modifiers = EVENTFLAG_NONE;

    if (wk.qual & QUAL_SHIFT)
        keyEvent.modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (wk.qual & QUAL_ALT)
        keyEvent.modifiers |= EVENTFLAG_ALT_DOWN;
    if (wk.qual & QUAL_CTRL)
        keyEvent.modifiers |= EVENTFLAG_CONTROL_DOWN;

    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);

    if (!superdown && eventData.Contains("ForceSuperDown"))
    {
        superdown = eventData["ForceSuperDown"].GetBool();
    }

    if (superdown)
        keyEvent.modifiers |= EVENTFLAG_COMMAND_DOWN;

    int darwinScanCode;
    if (SDLScanCodeToDarwinScanCode((SDL_Scancode) wk.scanCode, darwinScanCode))
    {
        keyEvent.native_key_code = darwinScanCode;
        keyEvent.type = wk.keyDown ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP;
        return true;
    }

    return false;
}

bool ConvertTextInputEvent(StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent)
{
    if (eventType != "TextInput")
    {
        ATOMIC_LOGERROR("ConvertTextInputEvent - Unknown event type");
        return false;
    }

    String text = eventData[TextInput::P_TEXT].GetString();

    SDL_Keycode keyCode = SDL_GetKeyFromName(text.CString());

    if (SDL_strlen(text.CString()) == 1)
    {
        if (text[0] >= 'A' && text[0] <= 'Z')
        {
            keyCode -= 32;
        }
    }

    keyEvent.character = (char16) keyCode;
    keyEvent.type = KEYEVENT_CHAR;

    return true;
}

}

#endif
