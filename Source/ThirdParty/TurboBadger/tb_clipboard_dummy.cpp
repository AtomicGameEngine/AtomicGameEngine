// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#include "tb_system.h"

#ifdef TB_CLIPBOARD_DUMMY

#include <sys/time.h>
#include <stdio.h>

#include "SDL.h"

namespace tb {

// == TBClipboard =====================================

void TBClipboard::Empty()
{
    SDL_SetClipboardText("");
}

bool TBClipboard::HasText()
{
    return SDL_HasClipboardText();
}

bool TBClipboard::SetText(const char *text)
{
    SDL_SetClipboardText(text);
    return true;
}

bool TBClipboard::GetText(TBStr &text)
{
    const char* _text = SDL_GetClipboardText();
    return text.Set(_text ? _text : "");
}

}; // namespace tb

#endif // TB_CLIPBOARD_DUMMY
