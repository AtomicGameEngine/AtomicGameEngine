// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#include <Atomic/Atomic.h>
#include "JSBType.h"
#include "JSBindings.h"


JSBType* JSBType::Parse(const String& value)
{
    JSBType* type = NULL;

    JSBindings* bindings = JSBindings::Instance();

    if (bindings->GetClass(value))
    {
        type = new JSBClassType(bindings->GetClass(value));
    }
    else if (bindings->GetEnum(value))
    {
        type = new JSBEnumType(bindings->GetEnum(value));
    }
    else if (value == "int")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Int);
    }
    else if (value == "bool")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Bool);
    }
    else if (value == "unsigned")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Int, true);

    }
    else if (value == "float")
    {
        type = new JSBPrimitiveType(JSBPrimitiveType::Float);
    }
    else if (value == "String")
    {
        type = new JSBStringType();

    }
    else if (value == "StringHash")
    {
        type = new JSBStringHashType();

    }
    return type;

}
