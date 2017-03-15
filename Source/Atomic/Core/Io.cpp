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

#include "../Precompiled.h"

#include "../Core/Io.h"

#ifdef ATOMIC_PLATFORM_WINDOWS

namespace Atomic
{

_locale_t GetCLocale()
{
    static _locale_t loc = _create_locale(LC_ALL, "C");
    return loc;
}

}

double Atomic_strtod(const char* nptr, char** endptr)
{
    return _strtod_l(nptr, endptr, Atomic::GetCLocale());
}

#elif defined(ATOMIC_PLATFORM_ANDROID) || defined(ATOMIC_PLATFORM_IOS)

// Android NDK/iOS don't do locale.

#else

// Linux and OSX share have the same implementation.

namespace Atomic
{

locale_t GetCLocale()
{
    static locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    return loc;
}

}

double Atomic_strtod(const char* nptr, char** endptr)
{
    return strtod_l(nptr, endptr, Atomic::GetCLocale());
}

#endif

// ATOMIC END
