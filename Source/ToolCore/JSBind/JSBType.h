//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

#include "JSBClass.h"
#include "JSBEnum.h"

namespace ToolCore
{

class JSBPrimitiveType;
class JSBStringType;
class JSBStringHashType;
class JSBClassType;
class JSBEnumType;
class JSBHeapPtrType;
class JSBVectorType;

class JSBType
{

public:

    // returns true if the types match
    virtual bool Match (JSBType* other) = 0;

    virtual JSBPrimitiveType* asPrimitiveType() { return 0; }
    virtual JSBClassType* asClassType() { return 0; }
    virtual JSBStringType* asStringType() { return 0; }
    virtual JSBStringHashType* asStringHashType() { return 0; }
    virtual JSBEnumType* asEnumType() { return 0; }
    virtual JSBHeapPtrType* asHeapPtrType() { return 0; }
    virtual JSBVectorType* asVectorType() { return 0; }

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

    virtual bool Match (JSBType* other)
    {
        if (!other)
            return false;

        const JSBPrimitiveType* pother = other->asPrimitiveType();

        if (!pother)
            return false;

        return (kind_ == pother->kind_ && isUnsigned_ == pother->isUnsigned_);

        return true;
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
            if (isUnsigned_)
                return "unsigned";
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

    virtual bool Match (JSBType* other)
    {
        if (!other)
            return false;

        return other->asStringType() == 0 ? false : true;
    }


};

class JSBStringHashType : public JSBType
{
public:

    virtual JSBStringHashType* asStringHashType() { return this; }

    String ToString() { return "StringHash"; }

    virtual bool Match (JSBType* other)
    {
        if (!other)
            return false;

        return other->asStringHashType() == 0 ? false : true;
    }

};

class JSBHeapPtrType : public JSBType
{
public:

    virtual JSBHeapPtrType* asHeapPtrType() { return this; }

    String ToString() { return "JS_HEAP_PTR"; }

    virtual bool Match (JSBType* other)
    {
        if (!other)
            return false;

        return other->asHeapPtrType() == 0 ? false : true;
    }

};


class JSBEnumType : public JSBType
{
public:

    JSBEnum* enum_;

    JSBEnumType(JSBEnum* penum) : enum_(penum) {}

    virtual JSBEnumType* asEnumType() { return this; }

    String ToString() { return enum_->GetName(); }

    virtual bool Match (JSBType* other)
    {
        if (!other)
            return false;

        JSBEnumType* pother = other->asEnumType();

        if (!pother || pother->enum_ != enum_)
            return false;

        return true;
    }

};

class JSBVectorType : public JSBType
{

public:

    JSBType* vectorType_;

    JSBVectorType(JSBType* vtype) : vectorType_(vtype) {}

    virtual JSBVectorType* asVectorType() { return this; }

    String ToString() { return "Vector<" + vectorType_->ToString() + ">"; }

    virtual bool Match (JSBType* other)
    {
        if (!other)
            return false;

        JSBVectorType* pother = other->asVectorType();

        if (!pother || !vectorType_->Match(pother->vectorType_))
            return false;

        return true;
    }

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
        return class_->GetNativeName();
    }

    virtual bool Match (JSBType* other)
    {
        if (!other)
            return false;

        JSBClassType* pother = other->asClassType();

        if (!pother || class_ != pother->class_)
            return false;

        return true;
    }


};

}
