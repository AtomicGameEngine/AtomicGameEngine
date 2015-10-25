
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
