//
// Copyright (c) 2008-2016 the Urho3D project.
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

// ATOMIC BEGIN

#pragma once

// The built-in strtod() function uses the current locale for parsing strings. On certain
// locales (German, Swedish, ...) a comma is used as decimal separator. For portable XML,
// we need to parse floating point values the same way on all platforms no matter what
// locale is set. The strtod_c_locale() is a workaround using the "C" locale explicitly,
// which uses a dot as decimal separator. For the same reason, sprintf() cannot be used.
// The function Atomic_strtod() and the macro Atomic_sprintf() should be used instead.

#ifdef ATOMIC_PLATFORM_WINDOWS

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

namespace Atomic
{
_locale_t GetCLocale();
}

double Atomic_strtod(const char* nptr, char** endptr);

#define Atomic_sprintf(buffer, format, ...)   _sprintf_l(buffer, format, ::Atomic::GetCLocale(), __VA_ARGS__)

#elif defined(ATOMIC_PLATFORM_ANDROID) || defined(ATOMIC_PLATFORM_IOS)

// Android NDK/iOS don't do locale.

#include <stdio.h>
#include <stdlib.h>

inline
double Atomic_strtod(const char* nptr, char** endptr)
{
    return strtod(nptr, endptr);
}

#define Atomic_sprintf(buffer, format, ...)   sprintf(buffer, format, __VA_ARGS__)

#elif defined(ATOMIC_PLATFORM_OSX)

#include <xlocale.h>
#include <stdio.h>
#include <stdlib.h>

namespace Atomic
{
locale_t GetCLocale();
}

double Atomic_strtod(const char* nptr, char** endptr);

#define Atomic_sprintf(buffer, format, ...)   sprintf_l(buffer, ::Atomic_GetCLocale(), format, __VA_ARGS__)

#else

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

namespace Atomic
{
locale_t GetCLocale();
}

double Atomic_strtod(const char* nptr, char** endptr);

#define Atomic_sprintf(buffer, format, ...)                            \
    do {                                                        \
        locale_t oldLocale = uselocale(::Atomic::GetCLocale()); \
        sprintf(buffer, format, __VA_ARGS__);                   \
        uselocale(oldLocale);                                   \
    } while (0)

#endif

// ATOMIC END
