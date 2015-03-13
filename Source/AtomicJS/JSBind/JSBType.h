// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/Str.h>
#include "JSBClass.h"

using namespace Atomic;

class JSBClass;
class JSBPrimitiveType;
class JSBStringType;
class JSBStringHashType;
class JSBClassType;
class JSBEnumType;
class JSBHeapPtrType;

class JSBType
{
public:

    virtual JSBPrimitiveType* asPrimitiveType() { return 0; }
    virtual JSBClassType* asClassType() { return 0; }
    virtual JSBStringType* asStringType() { return 0; }
    virtual JSBStringHashType* asStringHashType() { return 0; }
    virtual JSBEnumType* asEnumType() { return 0; }
    virtual JSBHeapPtrType* asHeapPtrType() { return 0; }

    static JSBType* Parse(const String& value);

    virtual String ToString() = 0;

};

class JSBPrimitiveType : public JSBType
{
public:
    // needs to match IntegerType::Kind
    enum Kind {
        Char,
        Char16,
        Char32,
        WideChar,
        Bool,
        Short,
        Int,
        Long,
        LongLong,
        Float // this doesn't exist in IntegerType::Kind
    };

    JSBPrimitiveType(int kind, bool isUnsigned = false)
    {

        kind_ = (Kind) kind;
        isUnsigned_ = isUnsigned;

    }

    String ToString()
    {
        switch (kind_)
        {
            case Char:
            case Char16:
            case Char32:
            case WideChar:
                return "char";
            case Bool:
                return "bool";
            case Short:
                return "short";
            case Int:
                return "int";
            case Long:
                return "long";
            case LongLong:
                return "long long";
            case Float:
                return "float";
        }

        return "???";
    }

    Kind kind_;
    bool isUnsigned_;

    virtual JSBPrimitiveType* asPrimitiveType() { return this; }

};

class JSBStringType : public JSBType
{
public:

    virtual JSBStringType* asStringType() { return this; }

    String ToString() { return "String"; }

};

class JSBStringHashType : public JSBType
{
public:

    virtual JSBStringHashType* asStringHashType() { return this; }

    String ToString() { return "StringHash"; }

};

class JSBHeapPtrType : public JSBType
{
public:

    virtual JSBHeapPtrType* asHeapPtrType() { return this; }

    String ToString() { return "JS_HEAP_PTR"; }

};


class JSBEnumType : public JSBType
{
public:

    JSBEnum* enum_;

    JSBEnumType(JSBEnum* penum) : enum_(penum) {}

    virtual JSBEnumType* asEnumType() { return this; }

    String ToString() { return enum_->name_; }

};


class JSBClassType : public JSBType
{
public:

    JSBClass* class_;

    virtual JSBClassType* asClassType() { return this; }

    JSBClassType(JSBClass* klass) : class_(klass)
    {

    }

    String ToString()
    {
        return class_->GetClassName();
    }

};
