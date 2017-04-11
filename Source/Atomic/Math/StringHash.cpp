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
}

StringHash::StringHash(const String& str) :
    value_(Calculate(str.CString()))
{
}

unsigned StringHash::Calculate(const char* str)
{
    unsigned hash = 0;

    if (!str)
        return hash;

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
static HashMap<unsigned, String> gSignificantLookup;

StringHash StringHash::RegisterSignificantString(const char* str)
{
    unsigned hash = Calculate(str);

    if (gSignificantLookup.Contains(hash))
        return StringHash(hash);

    gSignificantLookup[hash] = String(str);

    return StringHash(hash);

}

StringHash StringHash::RegisterSignificantString(const String& str)
{
    return RegisterSignificantString(str.CString());
}

bool StringHash::GetSignificantString(unsigned hash, String& strOut)
{
    if (!gSignificantLookup.TryGetValue(hash, strOut))
    {
        strOut.Clear();
        return false;
    }

    return true;

}

// ATOMIC END


}
