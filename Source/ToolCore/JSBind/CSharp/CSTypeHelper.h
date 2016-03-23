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

#include "../JSBFunction.h"

namespace ToolCore
{


class CSTypeHelper
{

public:

    // simple float, bool, int, uint
    // string

    // return value is out Parameter
    // "ArrayTypes" Vector, BoundingBox, etc

    // RefCounted*

    //JSBFunctionType*

    static void GenNativeFunctionParameterSignature(JSBFunction* function, String& sig);

    static String GetNativeFunctionSignature(JSBFunction* function, String& returnType);

    static bool IsSimpleReturn(JSBType* type);
    static bool IsSimpleReturn(JSBFunctionType* ftype);

    static String GetManagedPrimitiveType(JSBPrimitiveType* ptype);

    static String GetNativeTypeString(JSBType* type);
    static String GetNativeTypeString(JSBFunctionType* ftype);

    static String GetPInvokeTypeString(JSBType* type);
    static String GetPInvokeTypeString(JSBFunctionType* ftype);

    static String GetManagedTypeString(JSBType* type);
    static String GetManagedTypeString(JSBFunctionType* ftype, bool addName = true);

    static bool OmitFunction(JSBFunction* function);

};

/*
class JSBType
{

public:

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
*/


}
