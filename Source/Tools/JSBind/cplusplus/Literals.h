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

#ifndef CPLUSPLUS_LITERALS_H
#define CPLUSPLUS_LITERALS_H

#include "CPlusPlusForwardDeclarations.h"
#include "Token.h"
#include "Name.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT Literal
{
    Literal(const Literal &other);
    void operator =(const Literal &other);

public:
    typedef const char *iterator;
    typedef iterator const_iterator;

public:
    Literal(const char *chars, unsigned size);
    virtual ~Literal();

    iterator begin() const { return _chars; }
    iterator end() const { return _chars + _size; }

    char at(unsigned index) const { return _chars[index]; }
    const char *chars() const { return _chars; }
    unsigned size() const { return _size; }

    unsigned hashCode() const { return _hashCode; }
    static unsigned hashCode(const char *chars, unsigned size);

    bool equalTo(const Literal *other) const;

    Literal *_next; // ## private

private:
    char *_chars;
    unsigned _size;
    unsigned _hashCode;

public:
    unsigned _index;     // ### private
};

class CPLUSPLUS_EXPORT StringLiteral: public Literal
{
public:
    StringLiteral(const char *chars, unsigned size)
        : Literal(chars, size)
    { }
};

class CPLUSPLUS_EXPORT NumericLiteral: public Literal
{
public:
    NumericLiteral(const char *chars, unsigned size);

    enum {
        NumericLiteralIsInt,
        NumericLiteralIsFloat,
        NumericLiteralIsDouble,
        NumericLiteralIsLongDouble,
        NumericLiteralIsLong,
        NumericLiteralIsLongLong
    };

    bool isInt() const { return f._type == NumericLiteralIsInt; }
    bool isFloat() const { return f._type == NumericLiteralIsFloat; }
    bool isDouble() const { return f._type == NumericLiteralIsDouble; }
    bool isLongDouble() const { return f._type == NumericLiteralIsLongDouble; }
    bool isLong() const { return f._type == NumericLiteralIsLong; }
    bool isLongLong() const { return f._type == NumericLiteralIsLongLong; }
    bool isUnsigned() const { return f._isUnsigned; }
    bool isHex() const { return f._isHex; }

private:
    struct Flags {
        unsigned _type      : 8;
        unsigned _isHex     : 1;
        unsigned _isUnsigned: 1;
    };
    union {
        unsigned _flags;
        Flags f;
    };
};

class CPLUSPLUS_EXPORT Identifier: public Literal, public Name
{
public:
    Identifier(const char *chars, unsigned size)
        : Literal(chars, size)
    { }

    virtual const Identifier *identifier() const { return this; }

    virtual const Identifier *asNameId() const
    { return this; }

protected:
    virtual void accept0(NameVisitor *visitor) const;
    virtual bool match0(const Name *otherName, Matcher *matcher) const;
};

} // namespace CPlusPlus


#endif // CPLUSPLUS_LITERALS_H
