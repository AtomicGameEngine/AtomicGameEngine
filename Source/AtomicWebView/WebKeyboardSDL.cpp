#include <SDL/include/SDL_keycode.h>

#include "WebKeyboardSDL.h"

namespace Atomic
{

int GetNativeKeyFromSDLScanCode(int scancode)
{
#ifdef ATOMIC_PLATFORM_OSX
    // see scancodes_darwin.h in the SDL sources
    if (scancode == SDL_SCANCODE_RETURN)
        return 36;
    if (scancode == SDL_SCANCODE_LEFT)
        return 123;
    if (scancode == SDL_SCANCODE_RIGHT)
        return 124;
    if (scancode == SDL_SCANCODE_DOWN)
        return 125;
    if (scancode == SDL_SCANCODE_UP)
        return 126;
    if (scancode == SDL_SCANCODE_DELETE)
        return 117;
    if (scancode == SDL_SCANCODE_BACKSPACE)
        return 51;
    if (scancode == SDL_SCANCODE_RGUI)
        return 54;
    if (scancode == SDL_SCANCODE_LGUI)
        return 55;
    if (scancode == SDL_SCANCODE_Z)
        return 6;

    return -1;

#else

    return -1;

#endif

}

}
