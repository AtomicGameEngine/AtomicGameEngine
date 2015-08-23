#include <TurboBadger/tb_widgets.h>

using namespace tb;

#include "../Core/Timer.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"

#include "UI.h"
#include "UIEvents.h"

namespace Atomic
{

static MODIFIER_KEYS GetModifierKeys(int qualifiers, bool superKey)
{
    MODIFIER_KEYS code = TB_MODIFIER_NONE;
    if (qualifiers & QUAL_ALT)	code |= TB_ALT;
    if (qualifiers & QUAL_CTRL)	code |= TB_CTRL;
    if (qualifiers & QUAL_SHIFT)	code |= TB_SHIFT;
    if (superKey)	code |= TB_SUPER;
    return code;
}


// @return Return the upper case of a ascii charcter. Only for shortcut handling.
static int toupr_ascii(int ascii)
{
    if (ascii >= 'a' && ascii <= 'z')
        return ascii + 'A' - 'a';
    return ascii;
}

void UI::HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || consoleVisible_)
        return;

    using namespace MouseButtonDown;
    unsigned button = eventData[P_BUTTON].GetUInt();

    IntVector2 pos;
    pos = GetSubsystem<Input>()->GetMousePosition();

    Input* input = GetSubsystem<Input>();
    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif

    MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);


    static double last_time = 0;
    static int counter = 1;

    Time* t = GetSubsystem<Time>();

    double time = t->GetElapsedTime() * 1000;
    if (time < last_time + 600)
        counter++;
    else
        counter = 1;

    last_time = time;
    if (button == MOUSEB_RIGHT)
        rootWidget_->InvokeRightPointerDown(pos.x_, pos.y_, counter, mod);
    else
        rootWidget_->InvokePointerDown(pos.x_, pos.y_, counter, mod, false);


}


void UI::HandleMouseButtonUp(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || consoleVisible_)
        return;

    using namespace MouseButtonUp;
    unsigned button = eventData[P_BUTTON].GetUInt();

    IntVector2 pos;

    Input* input = GetSubsystem<Input>();
    pos = input->GetMousePosition();
    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif

    MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);


    if (button == MOUSEB_RIGHT)
        rootWidget_->InvokeRightPointerUp(pos.x_, pos.y_, mod);
    else
        rootWidget_->InvokePointerUp(pos.x_, pos.y_, mod, false);
}


void UI::HandleMouseMove(StringHash eventType, VariantMap& eventData)
{
    using namespace MouseMove;

    if (inputDisabled_ || consoleVisible_)
        return;

    int px = eventData[P_X].GetInt();
    int py = eventData[P_Y].GetInt();

    rootWidget_->InvokePointerMove(px, py, tb::TB_MODIFIER_NONE, false);

}


void UI::HandleMouseWheel(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || consoleVisible_)
        return;

    using namespace MouseWheel;

    int delta = eventData[P_WHEEL].GetInt();

    Input* input = GetSubsystem<Input>();

    rootWidget_->InvokeWheel(input->GetMousePosition().x_, input->GetMousePosition().y_, 0, delta > 0 ? -1 : 1, tb::TB_MODIFIER_NONE);

}

static bool InvokeShortcut(int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool down)
{
#ifdef __APPLE__
    bool shortcut_key = (modifierkeys & TB_SUPER) ? true : false;
#else
    bool shortcut_key = (modifierkeys & TB_CTRL) ? true : false;
#endif
    if (!TBWidget::focused_widget || !down || (!shortcut_key && special_key ==TB_KEY_UNDEFINED))
        return false;
    bool reverse_key = (modifierkeys & TB_SHIFT) ? true : false;
    int upper_key = toupr_ascii(key);
    TBID id;
    if (upper_key == 'X')
        id = TBIDC("cut");
    else if (upper_key == 'C' || special_key == TB_KEY_INSERT)
        id = TBIDC("copy");
    else if (upper_key == 'V' || (special_key == TB_KEY_INSERT && reverse_key))
        id = TBIDC("paste");
    else if (upper_key == 'A')
        id = TBIDC("selectall");
    else if (upper_key == 'Z' || upper_key == 'Y')
    {
        bool undo = upper_key == 'Z';
        if (reverse_key)
            undo = !undo;
        id = undo ? TBIDC("undo") : TBIDC("redo");
    }
    else if (upper_key == 'N')
        id = TBIDC("new");
    else if (upper_key == 'O')
        id = TBIDC("open");
    else if (upper_key == 'S')
        id = TBIDC("save");
    else if (upper_key == 'W')
        id = TBIDC("close");
    else if (upper_key == 'F')
        id = TBIDC("find");
 #ifdef ATOMIC_PLATFORM_OSX
    else if (upper_key == 'G' && (modifierkeys & TB_SHIFT))
        id = TBIDC("findprev");
    else if (upper_key == 'G')
        id = TBIDC("findnext");
#else
    else if (special_key == TB_KEY_F3 && (modifierkeys & TB_SHIFT))
        id = TBIDC("findprev");
    else if (special_key == TB_KEY_F3)
        id = TBIDC("findnext");

#endif
    else if (upper_key == 'P')
        id = TBIDC("play");
    else if (special_key == TB_KEY_PAGE_UP)
        id = TBIDC("prev_doc");
    else if (special_key == TB_KEY_PAGE_DOWN)
        id = TBIDC("next_doc");
    else
        return false;

    TBWidgetEvent ev(EVENT_TYPE_SHORTCUT);
    ev.modifierkeys = modifierkeys;
    ev.ref_id = id;

    TBWidget* eventWidget = TBWidget::focused_widget;

    if (id == TBIDC("save") || id == TBIDC("close")) {

        while (eventWidget && !eventWidget->GetDelegate()) {

            eventWidget = eventWidget->GetParent();
        }

    }

    if (!eventWidget)
        return false;

    return eventWidget->InvokeEvent(ev);
}

static bool InvokeKey(TBWidget* root, unsigned int key, SPECIAL_KEY special_key, MODIFIER_KEYS modifierkeys, bool keydown)
{
    if (InvokeShortcut(key, special_key, modifierkeys, keydown))
        return true;
    root->InvokeKey(key, special_key, modifierkeys, keydown);
    return true;
}


void UI::HandleKey(bool keydown, int keycode, int scancode)
{

#ifdef ATOMIC_PLATFORM_WINDOWS
    if (keycode == KEY_LCTRL || keycode == KEY_RCTRL)
        return;
#else
    if (keycode == KEY_LGUI || keycode == KEY_RGUI)
        return;
#endif

    Input* input = GetSubsystem<Input>();
    int qualifiers = input->GetQualifiers();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);
#endif
    MODIFIER_KEYS mod = GetModifierKeys(qualifiers, superdown);

    switch (keycode)
    {
    case KEY_RETURN:
    case KEY_RETURN2:
    case KEY_KP_ENTER:
        InvokeKey(rootWidget_, 0, TB_KEY_ENTER, mod, keydown);
        break;

    case KEY_F1:
        InvokeKey(rootWidget_, 0, TB_KEY_F1, mod, keydown);
        break;
    case KEY_F2:
        InvokeKey(rootWidget_, 0, TB_KEY_F2, mod, keydown);
        break;
    case KEY_F3:
        InvokeKey(rootWidget_, 0, TB_KEY_F3, mod, keydown);
        break;
    case KEY_F4:
        InvokeKey(rootWidget_, 0, TB_KEY_F4, mod, keydown);
        break;
    case KEY_F5:
        InvokeKey(rootWidget_, 0, TB_KEY_F5, mod, keydown);
        break;
    case KEY_F6:
        InvokeKey(rootWidget_, 0, TB_KEY_F6, mod, keydown);
        break;
    case KEY_F7:
        InvokeKey(rootWidget_, 0, TB_KEY_F7, mod, keydown);
        break;
    case KEY_F8:
        InvokeKey(rootWidget_, 0, TB_KEY_F8, mod, keydown);
        break;
    case KEY_F9:
        InvokeKey(rootWidget_, 0, TB_KEY_F9, mod, keydown);
        break;
    case KEY_F10:
        InvokeKey(rootWidget_, 0, TB_KEY_F10, mod, keydown);
        break;
    case KEY_F11:
        InvokeKey(rootWidget_, 0, TB_KEY_F11, mod, keydown);
        break;
    case KEY_F12:
        InvokeKey(rootWidget_, 0, TB_KEY_F12, mod, keydown);
        break;
    case KEY_LEFT:
        InvokeKey(rootWidget_, 0, TB_KEY_LEFT, mod, keydown);
        break;
    case KEY_UP:
        InvokeKey(rootWidget_, 0, TB_KEY_UP, mod, keydown);
        break;
    case KEY_RIGHT:
        InvokeKey(rootWidget_, 0, TB_KEY_RIGHT, mod, keydown);
        break;
    case KEY_DOWN:
        InvokeKey(rootWidget_, 0, TB_KEY_DOWN, mod, keydown);
        break;
    case KEY_PAGEUP:
        InvokeKey(rootWidget_, 0, TB_KEY_PAGE_UP, mod, keydown);
        break;
    case KEY_PAGEDOWN:
        InvokeKey(rootWidget_, 0, TB_KEY_PAGE_DOWN, mod, keydown);
        break;
    case KEY_HOME:
        InvokeKey(rootWidget_, 0, TB_KEY_HOME, mod, keydown);
        break;
    case KEY_END:
        InvokeKey(rootWidget_, 0, TB_KEY_END, mod, keydown);
        break;
    case KEY_INSERT:
        InvokeKey(rootWidget_, 0, TB_KEY_INSERT, mod, keydown);
        break;
    case KEY_TAB:
        InvokeKey(rootWidget_, 0, TB_KEY_TAB, mod, keydown);
        break;
    case KEY_DELETE:
        InvokeKey(rootWidget_, 0, TB_KEY_DELETE, mod, keydown);
        break;
    case KEY_BACKSPACE:
        InvokeKey(rootWidget_, 0, TB_KEY_BACKSPACE, mod, keydown);
        break;
    case KEY_ESC:
        InvokeKey(rootWidget_, 0, TB_KEY_ESC, mod, keydown);
        break;
    default:
        if (mod & TB_SUPER)
        {
            InvokeKey(rootWidget_, keycode, TB_KEY_UNDEFINED, mod, keydown);
        }

    }

}

void UI::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || keyboardDisabled_ || consoleVisible_)
        return;

    using namespace KeyDown;

    int keycode = eventData[P_KEY].GetInt();
    int scancode = eventData[P_SCANCODE].GetInt();

    HandleKey(true, keycode, scancode);

    // Send Global Shortcut
    Input* input = GetSubsystem<Input>();

#ifdef ATOMIC_PLATFORM_WINDOWS
    bool superdown = input->GetKeyDown(KEY_LCTRL) || input->GetKeyDown(KEY_RCTRL);
    if (keycode == KEY_LCTRL || keycode == KEY_RCTRL)
        superdown = false;
#else
    bool superdown = input->GetKeyDown(KEY_LGUI) || input->GetKeyDown(KEY_RGUI);

    if (keycode == KEY_LGUI || keycode == KEY_RGUI)
        superdown = false;
#endif

    if (!superdown)
        return;

    VariantMap shortcutData;
    shortcutData[UIShortcut::P_KEY] = keycode;
    shortcutData[UIShortcut::P_QUALIFIERS] = eventData[P_QUALIFIERS].GetInt();

    SendEvent(E_UISHORTCUT, shortcutData);

}

void UI::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || keyboardDisabled_ || consoleVisible_)
        return;

    using namespace KeyUp;

    int keycode = eventData[P_KEY].GetInt();
    int scancode = eventData[P_SCANCODE].GetInt();

    HandleKey(false, keycode, scancode);

}

void UI::HandleTextInput(StringHash eventType, VariantMap& eventData)
{
    if (inputDisabled_ || keyboardDisabled_ || consoleVisible_)
        return;

    using namespace TextInput;

    const String& text = eventData[P_TEXT].GetString();

    for (unsigned i = 0; i < text.Length(); i++)
    {
        InvokeKey(rootWidget_, text[i], TB_KEY_UNDEFINED, TB_MODIFIER_NONE, true);
        InvokeKey(rootWidget_, text[i], TB_KEY_UNDEFINED, TB_MODIFIER_NONE, false);
    }

}
}
