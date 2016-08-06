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

#include <ThirdParty/SDL/include/SDL.h>
#include <ThirdParty/SDL/include/SDL_syswm.h>
#include <ThirdParty/SDL/src/events/scancodes_windows.h>

namespace Atomic
{
 
// From ui/events/keycodes/keyboard_codes_posix.h.
enum KeyboardCode {
    VKEY_BACK = 0x08,       VKEY_TAB = 0x09,        VKEY_BACKTAB = 0x0A,        VKEY_CLEAR = 0x0C,
    VKEY_RETURN = 0x0D,     VKEY_SHIFT = 0x10,      VKEY_CONTROL = 0x11,        VKEY_MENU = 0x12,
    VKEY_PAUSE = 0x13,      VKEY_CAPITAL = 0x14,    VKEY_KANA = 0x15,           VKEY_HANGUL = 0x15,
    VKEY_JUNJA = 0x17,      VKEY_FINAL = 0x18,      VKEY_HANJA = 0x19,          VKEY_KANJI = 0x19,
    VKEY_ESCAPE = 0x1B,     VKEY_CONVERT = 0x1C,    VKEY_NONCONVERT = 0x1D,     VKEY_ACCEPT = 0x1E,
    VKEY_MODECHANGE = 0x1F, VKEY_SPACE = 0x20,      VKEY_PRIOR = 0x21,          VKEY_NEXT = 0x22,
    VKEY_END = 0x23,        VKEY_HOME = 0x24,       VKEY_LEFT = 0x25,           VKEY_UP = 0x26,
    VKEY_RIGHT = 0x27,      VKEY_DOWN = 0x28,       VKEY_SELECT = 0x29,         VKEY_PRINT = 0x2A,
    VKEY_EXECUTE = 0x2B,    VKEY_SNAPSHOT = 0x2C,   VKEY_INSERT = 0x2D,         VKEY_DELETE = 0x2E,
    VKEY_HELP = 0x2F,       VKEY_0 = 0x30,          VKEY_1 = 0x31,              VKEY_2 = 0x32,
    VKEY_3 = 0x33,          VKEY_4 = 0x34,          VKEY_5 = 0x35,              VKEY_6 = 0x36,
    VKEY_7 = 0x37,          VKEY_8 = 0x38,          VKEY_9 = 0x39,              VKEY_A = 0x41,
    VKEY_B = 0x42,          VKEY_C = 0x43,          VKEY_D = 0x44,              VKEY_E = 0x45,
    VKEY_F = 0x46,          VKEY_G = 0x47,          VKEY_H = 0x48,              VKEY_I = 0x49,
    VKEY_J = 0x4A,          VKEY_K = 0x4B,          VKEY_L = 0x4C,              VKEY_M = 0x4D,
    VKEY_N = 0x4E,          VKEY_O = 0x4F,          VKEY_P = 0x50,              VKEY_Q = 0x51,
    VKEY_R = 0x52,          VKEY_S = 0x53,          VKEY_T = 0x54,              VKEY_U = 0x55,
    VKEY_V = 0x56,          VKEY_W = 0x57,          VKEY_X = 0x58,              VKEY_Y = 0x59,
    VKEY_Z = 0x5A,          VKEY_LWIN = 0x5B,       VKEY_COMMAND = VKEY_LWIN,  // Provide the Mac name for convenience.
    VKEY_RWIN = 0x5C,       VKEY_APPS = 0x5D,       VKEY_SLEEP = 0x5F,          VKEY_NUMPAD0 = 0x60,
    VKEY_NUMPAD1 = 0x61,    VKEY_NUMPAD2 = 0x62,    VKEY_NUMPAD3 = 0x63,        VKEY_NUMPAD4 = 0x64,
    VKEY_NUMPAD5 = 0x65,    VKEY_NUMPAD6 = 0x66,    VKEY_NUMPAD7 = 0x67,        VKEY_NUMPAD8 = 0x68,
    VKEY_NUMPAD9 = 0x69,    VKEY_MULTIPLY = 0x6A,   VKEY_ADD = 0x6B,            VKEY_SEPARATOR = 0x6C,
    VKEY_SUBTRACT = 0x6D,   VKEY_DECIMAL = 0x6E,    VKEY_DIVIDE = 0x6F,         VKEY_F1 = 0x70,
    VKEY_F2 = 0x71,         VKEY_F3 = 0x72,         VKEY_F4 = 0x73,             VKEY_F5 = 0x74,
    VKEY_F6 = 0x75,         VKEY_F7 = 0x76,         VKEY_F8 = 0x77,             VKEY_F9 = 0x78,
    VKEY_F10 = 0x79,        VKEY_F11 = 0x7A,        VKEY_F12 = 0x7B,            VKEY_F13 = 0x7C,
    VKEY_F14 = 0x7D,        VKEY_F15 = 0x7E,        VKEY_F16 = 0x7F,            VKEY_F17 = 0x80,
    VKEY_F18 = 0x81,        VKEY_F19 = 0x82,        VKEY_F20 = 0x83,            VKEY_F21 = 0x84,
    VKEY_F22 = 0x85,        VKEY_F23 = 0x86,        VKEY_F24 = 0x87,            VKEY_NUMLOCK = 0x90,
    VKEY_SCROLL = 0x91,     VKEY_LSHIFT = 0xA0,     VKEY_RSHIFT = 0xA1,         VKEY_LCONTROL = 0xA2,
    VKEY_RCONTROL = 0xA3,   VKEY_LMENU = 0xA4,      VKEY_RMENU = 0xA5,          VKEY_BROWSER_BACK = 0xA6,
    VKEY_BROWSER_FORWARD = 0xA7,                    VKEY_BROWSER_REFRESH = 0xA8,VKEY_BROWSER_STOP = 0xA9,
    VKEY_BROWSER_SEARCH = 0xAA,                     VKEY_BROWSER_FAVORITES = 0xAB,  VKEY_BROWSER_HOME = 0xAC,
    VKEY_VOLUME_MUTE = 0xAD,VKEY_VOLUME_DOWN = 0xAE,VKEY_VOLUME_UP = 0xAF,      VKEY_MEDIA_NEXT_TRACK = 0xB0,
    VKEY_MEDIA_PREV_TRACK = 0xB1,                   VKEY_MEDIA_STOP = 0xB2,     VKEY_MEDIA_PLAY_PAUSE = 0xB3,
    VKEY_MEDIA_LAUNCH_MAIL = 0xB4,                  VKEY_MEDIA_LAUNCH_MEDIA_SELECT = 0xB5,
    VKEY_MEDIA_LAUNCH_APP1 = 0xB6,                  VKEY_MEDIA_LAUNCH_APP2 = 0xB7,
    VKEY_OEM_1 = 0xBA,      VKEY_OEM_PLUS = 0xBB,   VKEY_OEM_COMMA = 0xBC,      VKEY_OEM_MINUS = 0xBD,
    VKEY_OEM_PERIOD = 0xBE, VKEY_OEM_2 = 0xBF,      VKEY_OEM_3 = 0xC0,          VKEY_OEM_4 = 0xDB,
    VKEY_OEM_5 = 0xDC,      VKEY_OEM_6 = 0xDD,      VKEY_OEM_7 = 0xDE,          VKEY_OEM_8 = 0xDF,
    VKEY_OEM_102 = 0xE2,    VKEY_OEM_103 = 0xE3,  // GTV KEYCODE_MEDIA_REWIND
    VKEY_OEM_104 = 0xE4,  // GTV KEYCODE_MEDIA_FAST_FORWARD
    VKEY_PROCESSKEY = 0xE5, VKEY_PACKET = 0xE7,     VKEY_DBE_SBCSCHAR = 0xF3,   VKEY_DBE_DBCSCHAR = 0xF4,
    VKEY_ATTN = 0xF6,       VKEY_CRSEL = 0xF7,      VKEY_EXSEL = 0xF8,          VKEY_EREOF = 0xF9,
    VKEY_PLAY = 0xFA,       VKEY_ZOOM = 0xFB,       VKEY_NONAME = 0xFC,         VKEY_PA1 = 0xFD,
    VKEY_OEM_CLEAR = 0xFE,  VKEY_UNKNOWN = 0,
    // POSIX specific VKEYs. Note that as of Windows SDK 7.1, 0x97-9F, 0xD8-DA,
    // and 0xE8 are unassigned.
    VKEY_WLAN = 0x97,       VKEY_POWER = 0x98,      VKEY_BRIGHTNESS_DOWN = 0xD8,VKEY_BRIGHTNESS_UP = 0xD9,
    VKEY_KBD_BRIGHTNESS_DOWN = 0xDA,                VKEY_KBD_BRIGHTNESS_UP = 0xE8,
    // Windows does not have a specific key code for AltGr. We use the unused 0xE1
    // (VK_OEM_AX) code to represent AltGr, matching the behaviour of Firefox on
    // Linux.
    VKEY_ALTGR = 0xE1,
    // Windows does not have a specific key code for Compose. We use the unused
    // 0xE6 (VK_ICO_CLEAR) code to represent Compose.
    VKEY_COMPOSE = 0xE6,
};
   
static bool SDLScanCodeToWindowsScanCode(SDL_Scancode code, long int& lParam, long int& wParam )
{
    wParam = 0;
    lParam = 0;

    int numCodes = sizeof(windows_scancode_table)/sizeof(SDL_Scancode);

    int windowsScanCode = -1;
    for (int i  = 0; i < numCodes; i++)
    {
        if (windows_scancode_table[i] == code)
        {
            windowsScanCode = i;
            break;
        }
    }

    if (windowsScanCode != -1)
    {
        wParam = windowsScanCode;
        lParam = windowsScanCode << 16;
    }

    if ( code >= SDL_SCANCODE_A && code < SDL_SCANCODE_1 )
    {
        wParam = code + 61;
        lParam = wParam << 16;
        return wParam != 0 || lParam != 0;
    }
   
    switch (code)
    {
    case SDL_SCANCODE_RIGHT: wParam = VKEY_RIGHT; break;
    case SDL_SCANCODE_LEFT: wParam = VKEY_LEFT; break;
    case SDL_SCANCODE_UP: wParam = VKEY_UP; break;
    case SDL_SCANCODE_DOWN: wParam = VKEY_DOWN; break;
    case SDL_SCANCODE_DELETE: wParam = VKEY_DELETE; break;
    case SDL_SCANCODE_BACKSPACE: wParam = VKEY_BACK; break;
    case SDL_SCANCODE_TAB: wParam = VKEY_TAB; break;
    case SDL_SCANCODE_F1: wParam = VKEY_F1; break;
    case SDL_SCANCODE_F2: wParam = VKEY_F2; break;
    case SDL_SCANCODE_F3: wParam = VKEY_F3; break;
    case SDL_SCANCODE_F4: wParam = VKEY_F4; break;
    case SDL_SCANCODE_F5: wParam = VKEY_F5; break;
    case SDL_SCANCODE_F6: wParam = VKEY_F6; break;
    case SDL_SCANCODE_F7: wParam = VKEY_F7; break;
    case SDL_SCANCODE_F8: wParam = VKEY_F8; break;
    case SDL_SCANCODE_F9: wParam = VKEY_F9; break;
    case SDL_SCANCODE_F10: wParam = VKEY_F10; break;
    case SDL_SCANCODE_F11: wParam = VKEY_F11; break;
    case SDL_SCANCODE_F12: wParam = VKEY_F12; break;
    case SDL_SCANCODE_PRINTSCREEN: wParam = VKEY_SNAPSHOT; break;
    case SDL_SCANCODE_SCROLLLOCK: wParam = VKEY_SCROLL; break;
    case SDL_SCANCODE_PAUSE: wParam = VKEY_PAUSE; break;
    case SDL_SCANCODE_INSERT: wParam = VKEY_INSERT; break;
    case SDL_SCANCODE_HOME: wParam = VKEY_HOME; break;
    case SDL_SCANCODE_PAGEUP: wParam = VKEY_PRIOR; break;
    case SDL_SCANCODE_END: wParam = VKEY_END; break;
    case SDL_SCANCODE_PAGEDOWN: wParam = VKEY_NEXT; break;
    case SDL_SCANCODE_1: wParam = VKEY_1; break;
    case SDL_SCANCODE_2: wParam = VKEY_2; break;
    case SDL_SCANCODE_3: wParam = VKEY_3; break;
    case SDL_SCANCODE_4: wParam = VKEY_4; break;
    case SDL_SCANCODE_5: wParam = VKEY_5; break;
    case SDL_SCANCODE_6: wParam = VKEY_6; break;
    case SDL_SCANCODE_7: wParam = VKEY_7; break;
    case SDL_SCANCODE_8: wParam = VKEY_8; break;
    case SDL_SCANCODE_9: wParam = VKEY_9; break;
    case SDL_SCANCODE_0: wParam = VKEY_0; break;
    case SDL_SCANCODE_SEMICOLON: wParam = VKEY_OEM_1; break;
    case SDL_SCANCODE_APOSTROPHE: wParam = VKEY_OEM_7; break;
    case SDL_SCANCODE_GRAVE: wParam = VKEY_OEM_3; break;
    case SDL_SCANCODE_KP_DIVIDE: wParam = VKEY_DIVIDE; break;
    case SDL_SCANCODE_KP_MULTIPLY: wParam = VKEY_MULTIPLY; break;
    case SDL_SCANCODE_KP_MINUS: wParam = VKEY_SUBTRACT; break;
    case SDL_SCANCODE_KP_PLUS: wParam = VKEY_ADD; break;
    case SDL_SCANCODE_KP_ENTER: wParam = VKEY_SEPARATOR; break;
    case SDL_SCANCODE_KP_1: wParam = VKEY_NUMPAD1; break;
    case SDL_SCANCODE_KP_2: wParam = VKEY_NUMPAD2; break;
    case SDL_SCANCODE_KP_3: wParam = VKEY_NUMPAD3; break;
    case SDL_SCANCODE_KP_4: wParam = VKEY_NUMPAD4; break;
    case SDL_SCANCODE_KP_5: wParam = VKEY_NUMPAD5; break;
    case SDL_SCANCODE_KP_6: wParam = VKEY_NUMPAD6; break;
    case SDL_SCANCODE_KP_7: wParam = VKEY_NUMPAD7; break;
    case SDL_SCANCODE_KP_8: wParam = VKEY_NUMPAD8; break;
    case SDL_SCANCODE_KP_9: wParam = VKEY_NUMPAD9; break;
    case SDL_SCANCODE_KP_0: wParam = VKEY_NUMPAD0; break;
    case SDL_SCANCODE_KP_PERIOD: wParam = VKEY_DECIMAL; break;
    case SDL_SCANCODE_NUMLOCKCLEAR: wParam = VKEY_NUMLOCK; break;
    case SDL_SCANCODE_CAPSLOCK: wParam = VKEY_CAPITAL; break;
    }

    return wParam != 0 || lParam != 0;
}

bool ConvertKeyEvent(Input* input, const StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent)
{
    memset((void*)&keyEvent, 0, sizeof(keyEvent));
    
    WebKeyEvent wk(eventType, eventData);

    if (wk.scanCode == SDL_SCANCODE_RETURN)
    {
        if (!wk.keyDown)
            return false;

        keyEvent.character = VKEY_RETURN;
        keyEvent.windows_key_code = VKEY_RETURN;
        keyEvent.type = KEYEVENT_RAWKEYDOWN;
        return true;
    }

    long int lParam = 0, wParam = 0;

    keyEvent.modifiers = EVENTFLAG_NONE;

    if (wk.qual & QUAL_SHIFT)
        keyEvent.modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (wk.qual & QUAL_ALT)
        keyEvent.modifiers |= EVENTFLAG_ALT_DOWN;
    if (wk.qual & QUAL_CTRL)
        keyEvent.modifiers |= EVENTFLAG_CONTROL_DOWN;

    if (SDLScanCodeToWindowsScanCode((SDL_Scancode) wk.scanCode, lParam, wParam))
    {
        keyEvent.windows_key_code = (int) wParam;
        keyEvent.native_key_code = (int) lParam;
        keyEvent.type = wk.keyDown ? KEYEVENT_RAWKEYDOWN : KEYEVENT_KEYUP;
    }

    return true;
}

bool ConvertTextInputEvent(StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent)
{
   if (eventType != "TextInput")
    {
        ATOMIC_LOGERROR("ConvertTextInputEvent - Unknown event type");
        return false;
    }

    memset((void*)&keyEvent, 0, sizeof(keyEvent));

    String text = eventData[TextInput::P_TEXT].GetString();
 
    if ( text.Length() > 0 )
    {
        keyEvent.character = (char16)text[0];
        keyEvent.type = KEYEVENT_CHAR;
        return true; 
    }
    
    return false;
}

}

#endif
