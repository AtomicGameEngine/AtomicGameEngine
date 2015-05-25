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

#ifndef CPLUSPLUS_CORETYPES_H
#define CPLUSPLUS_CORETYPES_H

#include "CPlusPlusForwardDeclarations.h"
#include "Type.h"
#include "FullySpecifiedType.h"

namespace CPlusPlus {

class CPLUSPLUS_EXPORT UndefinedType : public Type
{
public:
    static UndefinedType *instance()
    {
        static UndefinedType t;
        return &t;
    }

    virtual const UndefinedType *asUndefinedType() const
    { return this; }

    virtual UndefinedType *asUndefinedType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;
};

class CPLUSPLUS_EXPORT VoidType: public Type
{
public:
    virtual const VoidType *asVoidType() const
    { return this; }

    virtual VoidType *asVoidType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;
};

class CPLUSPLUS_EXPORT IntegerType: public Type
{
public:
    enum Kind {
        Char,
        Char16,
        Char32,
        WideChar,
        Bool,
        Short,
        Int,
        Long,
        LongLong
    };

public:
    IntegerType(int kind);
    virtual ~IntegerType();

    int kind() const;

    virtual IntegerType *asIntegerType()
    { return this; }

    virtual const IntegerType *asIntegerType() const
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    int _kind;
};

class CPLUSPLUS_EXPORT FloatType: public Type
{
public:
    enum Kind {
        Float,
        Double,
        LongDouble
    };

public:
    FloatType(int kind);
    virtual ~FloatType();

    int kind() const;

    virtual const FloatType *asFloatType() const
    { return this; }

    virtual FloatType *asFloatType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    int _kind;
};

class CPLUSPLUS_EXPORT PointerType: public Type
{
public:
    PointerType(const FullySpecifiedType &elementType);
    virtual ~PointerType();

    FullySpecifiedType elementType() const;

    virtual const PointerType *asPointerType() const
    { return this; }

    virtual PointerType *asPointerType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    FullySpecifiedType _elementType;
};

class CPLUSPLUS_EXPORT PointerToMemberType: public Type
{
public:
    PointerToMemberType(const Name *memberName, const FullySpecifiedType &elementType);
    virtual ~PointerToMemberType();

    const Name *memberName() const;
    FullySpecifiedType elementType() const;

    virtual const PointerToMemberType *asPointerToMemberType() const
    { return this; }

    virtual PointerToMemberType *asPointerToMemberType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    const Name *_memberName;
    FullySpecifiedType _elementType;
};

class CPLUSPLUS_EXPORT ReferenceType: public Type
{
public:
    ReferenceType(const FullySpecifiedType &elementType, bool rvalueRef);
    virtual ~ReferenceType();

    FullySpecifiedType elementType() const;
    bool isRvalueReference() const;

    virtual const ReferenceType *asReferenceType() const
    { return this; }

    virtual ReferenceType *asReferenceType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    FullySpecifiedType _elementType;
    bool _rvalueReference;
};

class CPLUSPLUS_EXPORT ArrayType: public Type
{
public:
    ArrayType(const FullySpecifiedType &elementType, unsigned size);
    virtual ~ArrayType();

    FullySpecifiedType elementType() const;
    unsigned size() const;

    virtual const ArrayType *asArrayType() const
    { return this; }

    virtual ArrayType *asArrayType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    FullySpecifiedType _elementType;
    unsigned _size;
};

class CPLUSPLUS_EXPORT NamedType: public Type
{
public:
    NamedType(const Name *name);
    virtual ~NamedType();

    const Name *name() const;

    virtual const NamedType *asNamedType() const
    { return this; }

    virtual NamedType *asNamedType()
    { return this; }

protected:
    virtual void accept0(TypeVisitor *visitor);
    virtual bool match0(const Type *otherType, Matcher *matcher) const;

private:
    const Name *_name;
};

} // namespace CPlusPlus

#endif // CPLUSPLUS_CORETYPES_H
