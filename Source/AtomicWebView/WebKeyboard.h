
#pragma once

#include <Atomic/Core/Variant.h>

namespace Atomic
{

class Input;

bool ConvertKeyEvent(Input* input, const StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent);
bool ConvertTextInputEvent(const StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent);

struct WebKeyEvent
{
    bool repeat;
    int key;
    int scanCode;
    unsigned raw;
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
            raw = eventData[KeyDown::P_RAW].GetUInt();
            buttons = eventData[KeyDown::P_BUTTONS].GetInt();
            qual = eventData[KeyDown::P_QUALIFIERS].GetInt();
        }
        else if (eventType == "KeyUp")
        {
            keyDown = false;
            key = eventData[KeyUp::P_KEY].GetInt();
            scanCode = eventData[KeyUp::P_SCANCODE].GetInt();
            raw = eventData[KeyUp::P_RAW].GetUInt();
            buttons = eventData[KeyUp::P_BUTTONS].GetInt();
            qual = eventData[KeyUp::P_QUALIFIERS].GetInt();
        }

    }
};

}
