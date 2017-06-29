//
// Copyright (c) 2008-2017 the Urho3D project.
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

#include "../Precompiled.h"

#include "../Math/MathDefs.h"
#include "../Math/StringHash.h"

#include <cstdio>

#include "../DebugNew.h"

// ATOMIC BEGIN

#include "../Container/HashMap.h"

// ATOMIC END

namespace Atomic
{

const StringHash StringHash::ZERO;

StringHash::StringHash(const char* str) :
    value_(Calculate(str))
{
#if ATOMIC_PROFILING
    RegisterSignificantString(str, *this);
#endif
}

StringHash::StringHash(const String& str) :
    value_(Calculate(str.CString()))
{
#if ATOMIC_PROFILING
    RegisterSignificantString(str, *this);
#endif
}
// ATOMIC BEGIN
unsigned StringHash::Calculate(const char* str, unsigned hash)
{
    if (!str)
        return hash;
// ATOMIC END
    while (*str)
    {
        // Perform the actual hashing as case-insensitive
        char c = *str;
        hash = SDBMHash(hash, (unsigned char)tolower(c));
        ++str;
    }

    return hash;
}

String StringHash::ToString() const
{
    char tempBuffer[CONVERSION_BUFFER_LENGTH];
    sprintf(tempBuffer, "%08X", value_);
    return String(tempBuffer);
}

// ATOMIC BEGIN

// Lookup for significant strings, not a member of StringHash so don't need to drag hashmap into header
static HashMap<StringHash, String>* gSignificantLookup = 0;

StringHash StringHash::RegisterSignificantString(const String& str)
{
    StringHash hash(str.CString());
    RegisterSignificantString(str.CString(), hash);
    return hash;
}

void StringHash::RegisterSignificantString(const char* str, StringHash hash)
{
    if (!gSignificantLookup)
        gSignificantLookup = new HashMap<StringHash, String>();

    if (gSignificantLookup->Contains(hash))
        return;

    (*gSignificantLookup)[hash] = str;
}

StringHash StringHash::RegisterSignificantString(const char* str)
{
    StringHash hash(str);
    RegisterSignificantString(str, hash);
    return hash;
}

void StringHash::RegisterSignificantString(const String& str, StringHash hash)
{
    RegisterSignificantString(str.CString(), hash);
}

bool StringHash::GetSignificantString(StringHash hash, String& strOut)
{
    if (!gSignificantLookup || !gSignificantLookup->TryGetValue(hash, strOut))
    {
        strOut.Clear();
        return false;
    }

    return true;
}

// ATOMIC END


}
