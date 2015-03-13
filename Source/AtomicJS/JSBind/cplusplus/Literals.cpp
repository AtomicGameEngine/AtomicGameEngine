// Copyright (c) 2008 Roberto Raggi <roberto.raggi@gmail.com>
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

#include "Literals.h"
#include "NameVisitor.h"
#include "Matcher.h"
#include <cstring>
#include <algorithm>
#include <iostream>

using namespace CPlusPlus;

////////////////////////////////////////////////////////////////////////////////
Literal::Literal(const char *chars, unsigned size)
    : _next(0), _index(0)
{
    _chars = new char[size + 1];

    std::strncpy(_chars, chars, size);
    _chars[size] = '\0';
    _size = size;

    _hashCode = hashCode(_chars, _size);
}

Literal::~Literal()
{ delete[] _chars; }

bool Literal::equalTo(const Literal *other) const
{
    if (! other)
        return false;
    else if (this == other)
        return true;
    else if (hashCode() != other->hashCode())
        return false;
    else if (size() != other->size())
        return false;
    return ! std::strcmp(chars(), other->chars());
}



unsigned Literal::hashCode(const char *chars, unsigned size)
{
    /* Hash taken from QtCore's qHash for strings, which in turn has the note:

    These functions are based on Peter J. Weinberger's hash function
    (from the Dragon Book). The constant 24 in the original function
    was replaced with 23 to produce fewer collisions on input such as
    "a", "aa", "aaa", "aaaa", ...
    */

    unsigned h = 0;

    while (size--) {
        h = (h << 4) + *chars++;
        h ^= (h & 0xf0000000) >> 23;
        h &= 0x0fffffff;
    }
    return h;
}

////////////////////////////////////////////////////////////////////////////////
NumericLiteral::NumericLiteral(const char *chars, unsigned size)
    : Literal(chars, size), _flags(0)
{
    f._type = NumericLiteralIsInt;

    if (size > 1 && chars[0] == '0' && (chars[1] == 'x' || chars[1] == 'X')) {
        f._isHex = true;
    } else {
        const char *begin = chars;
        const char *end = begin + size;

        bool done = false;
        const char *it = end - 1;

        for (; it != begin - 1 && ! done; --it) {
            switch (*it) {
            case 'l': case 'L': // long suffix
            case 'u': case 'U': // unsigned suffix
            case 'f': case 'F': // floating suffix
                break;

            default:
                done = true;
                break;
            } // switch
        }

        for (const char *dot = it; it != begin - 1; --it) {
            if (*dot == '.')
                f._type = NumericLiteralIsDouble;
        }

        for (++it; it != end; ++it) {
            if (*it == 'l' || *it == 'L') {
                if (f._type == NumericLiteralIsDouble) {
                    f._type = NumericLiteralIsLongDouble;
                } else if (it + 1 != end && (it[1] == 'l' || it[1] == 'L')) {
                    ++it;
                    f._type = NumericLiteralIsLongLong;
                } else {
                    f._type = NumericLiteralIsLong;
                }
            } else if (*it == 'f' || *it == 'F') {
                f._type = NumericLiteralIsFloat;
            } else if (*it == 'u' || *it == 'U') {
                f._isUnsigned = true;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void Identifier::accept0(NameVisitor *visitor) const
{ visitor->visit(this); }

bool Identifier::match0(const Name *otherName, Matcher *matcher) const
{
    if (const Identifier *id = otherName->asNameId())
        return matcher->match(this, id);
    return false;
}
