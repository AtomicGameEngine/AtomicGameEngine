
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
    return false;
}

bool ConvertTextInputEvent(StringHash eventType, VariantMap& eventData, CefKeyEvent& keyEvent)
{
    return false;
}

}

#endif
